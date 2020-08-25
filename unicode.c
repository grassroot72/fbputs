/*
 * Copyright (C) 2019  Edward LEI <edward_lei72@hotmail.com>
 *
 * The code is licensed under the MIT license
 */

#include <stdio.h>

#include "conf.h"
#include "unicode.h"


/*----- Internally used variables & functions --------------------------------*/

/* Is c the start of a UTF-8 sequence? */
#define ISUTF8(c) ((c & 0xc0) != 0x80)

/* Double width character specific */
static int _dwchars[][2] = {
  {0x1100, 0x115f}, {0x11a3, 0x11a7}, {0x11fa, 0x11ff}, {0x2329, 0x232a},
  {0x2e80, 0x2e99}, {0x2e9b, 0x2ef3}, {0x2f00, 0x2fd5}, {0x2ff0, 0x2ffb},
  {0x3000, 0x3029}, {0x3030, 0x303e}, {0x3041, 0x3096}, {0x309b, 0x30ff},
  {0x3105, 0x312d}, {0x3131, 0x318e}, {0x3190, 0x31b7}, {0x31c0, 0x31e3},
  {0x31f0, 0x321e}, {0x3220, 0x3247}, {0x3250, 0x32fe}, {0x3300, 0x4dbf},
  {0x4e00, 0xa48c}, {0xa490, 0xa4c6}, {0xa960, 0xa97c}, {0xac00, 0xd7a3},
  {0xd7b0, 0xd7c6}, {0xd7cb, 0xd7fb}, {0xf900, 0xfaff}, {0xfe10, 0xfe19},
  {0xfe30, 0xfe52}, {0xfe54, 0xfe66}, {0xfe68, 0xfe6b}, {0xff01, 0xff60},
  {0xffe0, 0xffe6}
};

static int _dwFind(int c, int n)
{
  int l = 0;
  int h = n - 1;
  int m;

  if (c < _dwchars[0][0]) return 0;

  while (l <= h) {
    m = (h + l) / 2;
    if (c >= _dwchars[m][0] && c <= _dwchars[m][1])
      return 1;
    if(c < _dwchars[m][0])
      h = m - 1;
    else
      l = m + 1;
  }

  return 0;
}

/* UTF-8 specific */
static const u32_t _UTF8offsets[6] = {
   0x00000000, 0x00003080, 0x000e2080,
   0x03c82080, 0xfa082080, 0x82082080
};

static const u8_t _UTF8tailbytes[256] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Convert a UTF-8 buffer to a UTF-32 buffer
 *   u8len - length in s8_t characters
 *
 * NOTE:
 * only works for valid UTF-8 sequences,
 * not for 5- or 6-byte sequences
 *
 * Return: number of characters converted
 */
static u32_t _ucU8ToU32(u32_t *u32buf, s8_t *u8buf, u32_t u8len)
{
  s8_t *u8ps;      /* pointer start */
  s8_t *u8pe;      /* pointer end */
  u32_t ch;

  u32_t nb;        /* number of bytes */
  u32_t i;

  u8ps = u8buf;
  u8pe = u8ps + u8len;

  i = 0;

  if (u8len == 0) return 0;

  while (i < u8len) {
    if (!ISUTF8(*u8ps)) {      /* invalid sequence */
      u32buf[i++] = 0xfffd;
      u8ps++;
      if (u8ps >= u8pe) break;
      continue;
    }

    nb = _UTF8tailbytes[(u8_t)*u8ps];
    if (u8ps + nb >= u8pe) break;

    ch = 0;
    switch (nb) {
      /* these fall through deliberately */
      case 5: ch += (u8_t)*u8ps++; ch <<= 6;
      case 4: ch += (u8_t)*u8ps++; ch <<= 6;
      case 3: ch += (u8_t)*u8ps++; ch <<= 6;
      case 2: ch += (u8_t)*u8ps++; ch <<= 6;
      case 1: ch += (u8_t)*u8ps++; ch <<= 6;
      case 0: ch += (u8_t)*u8ps++;
    }

    ch -= _UTF8offsets[nb];
    u32buf[i++] = ch;
  }

  return i;
}

/*
 * Convert a UTF-32 buffer to a UTF-16 buffer
 *
 * NOTE:
 * no UTF-16 surrogate considered
 */
static void _ucU32ToU16(u16_t *u16buf, u32_t *u32buf, u32_t len)
{
  u32_t i;

  for (i = 0; i < len; i++) {
    u16buf[i] = u32buf[i];
  }
}

/*
 * Check the BOM (Byte Order Mark)
 *
 * Return:  0 - UTF-16 BOM
 *          1 - part of UTF-8 BOM
 *          -1 - invalid unicode BOM
 */
int _ucBOMChk(u16_t ch)
{
  /* UTF-16 BOM, 0xFEFF (little endian) */
  if (ch == 0xfeff) {
    return 0;
  }
  /*
   * The UTF-8 BOM, 0xEFBBBF (little endian)
   *
   * To read the first 2 bytes of UTF-8 BOM as a UTF-16 character,
   * it should be read as 0xBBEF
   */
  else if (ch == 0xbbef) {
    return 1;
  }
  else {
    fprintf(stderr, "Invalid unicode BOM\n");
    return -1;
  }
}

/*
 * Convert a UTF-8 buffer to a UTF-16 buffer
 */
u32_t _ucU8ToU16(u16_t *u16buf, s8_t *u8buf, u32_t u8len)
{
  u32_t u32buf[U32_NCHARS];
  u32_t u32n;

  u32n = _ucU8ToU32(u32buf, u8buf, u8len);

  _ucU32ToU16(u16buf, u32buf, u32n);

  return u32n;
}

/*----- End: Internally used variables & functions ---------------------------*/


int ucIsDW(u16_t c)
{
  return c >= 0x1100 && _dwFind(c, LEN(_dwchars));
}

u32_t ucLoadToU16(u16_t *u16buf, char *fname)
{
  u16_t ch;
  u32_t u16n;

  s8_t u8buf[U8_NCHARS];
  s8_t u8ch;
  u8_t u8bom;
  u32_t u8n;

  int bomchk;

  FILE *fp;

  fp = fopen(fname, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Cannot open file %s\n", fname);
    return 0;
  }

  /* read a u16_char to check the BOM */
  fread(&ch, sizeof(u16_t), 1, fp);
  /* check the BOM */
  bomchk = _ucBOMChk(ch);

  if (bomchk == 0) {
    u16n = 0;

    while (fread(&ch, sizeof(u16_t), 1, fp) == 1) {
      u16buf[u16n++] = ch;
    }
    u16buf[u16n++] = '\0';
    fclose(fp);

#ifdef UNICODE_INFO
    fprintf(stdout, "UTF-16 encoded, %d characters\n", u16n);
#endif

    return u16n;
  }
  else if (bomchk == 1) {
    fread(&u8bom, sizeof(u8_t), 1, fp);   /* UTF-8 BOM last byte: 0xBF */
    if (u8bom != 0xbf) {
      fprintf(stderr, "not a valid UTF-8 file\n");
      fclose(fp);
      return 0;
    }

    u8n = 0;

    while(fread(&u8ch, sizeof(s8_t), 1, fp) == 1) {
      u8buf[u8n++] = u8ch;
    }
    u8buf[u8n++] = '\0';
    fclose(fp);

#ifdef UNICODE_INFO
    fprintf(stdout, "UTF-8 encoded, %d characters, ", u8n);
#endif

    u16n = _ucU8ToU16(u16buf, u8buf, u8n);

#ifdef UNICODE_INFO
    fprintf(stdout, "converted to UTF-16 encoding, %d characters\n", u16n);
#endif

    return u16n;
  }
  else {
    fprintf(stderr, "File not loaded!\n");
    return 0;
  }
}
