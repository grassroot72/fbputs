/*
  font.c

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "conf.h"
#include "fb.h"
#include "font.h"


/*----- Internally used variables & functions --------------------------------*/

struct _freetype2_font_s {
  FT_Library lib;        /* handle to library */
  FT_Face sface;         /* handle to face object */
  FT_Face dface;         /* handle to face object */
  FT_Face sbface;        /* handle to face (bold) object */
  FT_Face dbface;        /* handle to face (bold) object */

  int ascender;          /* maximum ascender */
  int descender;         /* minimun descender, that is max abs(descender) */

  /* 
    The terminal screen is filled with characters. A single width character
    takes a single cell whilst a double width character takes 2 consecutive
    cells.
  */
  int h;                 /* single cell height in pixels */
  int w;                 /* single cell width in pixels */

  /* origin is the upper left corner */
  u8_t img[CELLH][CELLW];
};

/* Note: I know it is not a good practice to declare avariable _ft at this
         scope and this makes the code quite cohesive, but I couldn't refuse
         the following benefits,
   (a). convenient to protect struct _freetype2_font_s's data members
   (b). shorter function code (if you believe ... :)
*/
static struct _freetype2_font_s _ft;


/*
  Set the text line height 
   h - text line height in pixels (both sfnt and dfnt)
*/
static int _ftSetH(int h)
{
  int s_ascender, s_descender;      /* for single width character */
  int d_ascender, d_descender;      /* for double width character */

  FT_Set_Pixel_Sizes(_ft.sface, 0, h);
  FT_Set_Pixel_Sizes(_ft.dface, 0, h);
  if (_ft.sbface) FT_Set_Pixel_Sizes(_ft.sbface, 0, h);
  if (_ft.dbface) FT_Set_Pixel_Sizes(_ft.dbface, 0, h);

  s_ascender = _ft.sface->size->metrics.ascender;
  s_descender = _ft.sface->size->metrics.descender;

  d_ascender = _ft.dface->size->metrics.ascender;
  d_descender = _ft.dface->size->metrics.descender;

  _ft.ascender = MAX(s_ascender, d_ascender) >> 6;
  _ft.descender = MIN(s_descender, d_descender) >> 6;

  /* my own height calculation, not even comply with the theory, but works */
  _ft.h = ((h + _ft.ascender - _ft.descender) >> 1) + XLSPACE;
  if (_ft.h > CELLH) {
    fprintf(stderr, "FreeType: exceed maximum font height: %d\n", h);
    return 1;
  }

  _ft.w = _ft.sface->size->metrics.max_advance >> 6;

  /*
  --> Debug: ascender, descender
  fprintf(stderr, "[ single cell height = %d ]\n", _ft.h);
  fprintf(stderr, "[ single cell width  = %d ]\n", _ft.w);
  fprintf(stderr, "[ ascender  = %d ]\n", _ft.ascender);
  fprintf(stderr, "[ descender = %d ]\n", _ft.descender);
  --> Debug
  */

  return 0;
}

/*
  Generate a character bitmap on image buffer
*/
static void _ftBmp(FT_Bitmap *bmp, FT_Int x, FT_Int y)
{
  FT_Int i, j, p, q;
  FT_Int xmax = x + bmp->width;
  FT_Int ymax = y + bmp->rows;

  /*
    for simplicity, we assume that 'bitmap->pixel_mode'
    is 'FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)
  */
  for (i = x, p = 0; i < xmax; i++, p++) {
    for (j = y, q = 0; j < ymax; j++, q++) {
      if (i < 0 || j < 0 || i >= CELLW || j >= CELLH)
        continue;
      _ft.img[j][i] |= bmp->buffer[q * bmp->width + p];
    }
  }
}

/*
  Load a single character
*/
static void _ftLoadCh(FT_Face face, FT_UInt g_idx)
{
  /* load glyph image into the slot (erase previous one) */
  if (FORCEHINT)
    FT_Load_Glyph(face, g_idx, FT_LOAD_FORCE_AUTOHINT);
  else
    FT_Load_Glyph(face, g_idx, FT_LOAD_DEFAULT);

  /* convert to an anti-aliased bitmap */
  FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
}

/*
  Convert a character to bitmap
   ch - UTF-16 character
   dw - double width character?
   bold - bold
*/
static void _ftCh2Bmp(u16_t ch, int dw, int bold)
{
  FT_Face face;        /* handle to face object */
  FT_GlyphSlot slot;   /* a small shortcut */
  FT_UInt g_idx;       /* glyph index */
  int _x, _y;          /* pen start postion x, y */

  _x = 0;
  _y = _ft.h + _ft.descender;   /* the pen's y position */

  /* double width character, CJK ... */
  if (dw) {
    face = _ft.dface;
    if (bold && _ft.dbface) face = _ft.dbface;

    /* retrieve glyph index from character code */
    g_idx = FT_Get_Char_Index(face, ch);

    _ftLoadCh(face, g_idx);
    slot = face->glyph;  /* a small shortcut */
  }
  /* single width character, ASCII ... */
  else {
    face = _ft.sface;
    if (bold && _ft.sbface) face = _ft.sbface;

    /* retrieve glyph index from character code */
    g_idx = FT_Get_Char_Index(face, ch);

    _ftLoadCh(face, g_idx);
    slot = face->glyph;  /* a small shortcut */
  }

  _ftBmp(&slot->bitmap, _x + slot->bitmap_left, _y - slot->bitmap_top);
}

/*----- End: Internally used variables & functions ---------------------------*/


int ftLoad(char *sfnt, char *dfnt, char *sbfnt, char *dbfnt, int h)
{
  FT_Error err;

  err = FT_Init_FreeType(&_ft.lib);
  if (err) {
    fprintf(stderr, "FreeType: failed to initialize the library\n");
    return 1;
  }

  /* single width character font */
  err = FT_New_Face(_ft.lib, sfnt, 0, &_ft.sface);
  if (err == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "FreeType: unkown file format: %s\n", sfnt);
    return 1;
  }
  else if (err) {
    fprintf(stderr, "FreeType: failed to get face for %s\n", sfnt);
    return 1;
  }

  /* double width character supported font */
  err = FT_New_Face(_ft.lib, dfnt, 0, &_ft.dface);
  if (err == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "FreeType: unkown file format: %s\n", dfnt);
    return 1;
  }
  else if (err) {
    fprintf(stderr, "FreeType: failed to get face for %s\n", dfnt);
    return 1;
  }

  /* single width character (bold) font */
  err = FT_New_Face(_ft.lib, sbfnt, 0, &_ft.sbface);
  if (err == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "FreeType: unkown file format: %s\n", sbfnt);
    return 1;
  }
  else if (err) {
    _ft.sbface = 0;
  }

  /* double width character (bold) supported font */
  err = FT_New_Face(_ft.lib, dbfnt, 0, &_ft.dbface);
  if (err == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "FreeType: unkown file format: %s\n", dfnt);
    return 1;
  }
  else if (err) {
    _ft.dbface = 0;
  }

  _ftSetH(h);

  return 0;
}

void ftFree()
{
  FT_Done_Face(_ft.sface);
  FT_Done_Face(_ft.dface);
  FT_Done_Face(_ft.sbface);
  FT_Done_Face(_ft.dbface);
  FT_Done_FreeType(_ft.lib);
}

int ftH()
{
  return _ft.h;
}

int ftW()
{
  return _ft.w;
}

void ftCh2FB(int r, int c, u16_t ch, int dw, int b, fb32_t fg, fb32_t bg)
{
  static fb32_t line[CELLW];  /* pixel line */
  int pr;                     /* pixel start row number */
  int pc;                     /* pixel start column number */
  int w;                      /* actual cell(s) width in pixels */
  int bsz;                    /* bsz - bitmap line size */
  int i, j;
  fb32_t color;

  _ftCh2Bmp(ch, dw, b);

  pr = _ft.h * r;
  pc = _ft.w * c;

  if(dw)
    w = _ft.w << 1;           /* w = _ft.w + _ft.w; */
  else
    w = _ft.w;

  bsz = w * sizeof(u8_t);

  for (i = 0; i < _ft.h; i++) {
    for (j = 0; j < w; j++ ) {
      color = _ft.img[i][j];
      line[j] = fbMixedColor(fg, bg, color);
    }
    fbSet(i + pr, pc, line, w);
    memset(_ft.img[i], 0, bsz);
  }
}