/*
  fb.c

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "conf.h"
#include "fb.h"


/*----- Internally used variables & functions --------------------------------*/

#define R(c)   ((c >> 16) & 0x0000ff)
#define G(c)   ((c >> 8) & 0x0000ff)
#define B(c)   (c & 0x0000ff)
#define COLORMIX(fg, bg, c)   (bg + ((fg - bg) * c >> 8u))


struct _framebuffer_s {
  int fd;                    /* FB device file descriptor */
  void *mem;                 /* FB memory start address */
  u32_t xoffset;             /* xoffset from virtual to visible resolution */
  u32_t yoffset;             /* yoffset from virtual to visible resolution */
  int bpp;                   /* bytes per pixel */
  u32_t w;                   /* FB screen resolution x */
  u32_t h;                   /* FB screen resolution y */
  u32_t llen;                /* FB line length */
  u32_t len;                 /* FB total length */
  int nr, ng, nb;            /* color levels */
  int rl, rr;                /* red left, right shifts */
  int gl, gr;                /* green left, right shifts */
  int bl, br;                /* blue left, right shifts */
};

/*
  NOTE:
  I know it is not a good practice to declare avariable _fb at this
  scope and this makes the code quite cohesive, but I couldn't refuse
  the following benefits,
  (a). convenient to protect struct _framebuffer_s's data members
  (b). shorter function code (if you believe ... :)
*/
static struct _framebuffer_s _fb;

/*----- End: Internally used variables & functions ---------------------------*/



int fbInit(char *dev)
{
  struct fb_var_screeninfo vinfo;   /* linux FB structure */
  struct fb_fix_screeninfo finfo;   /* linux FB structure */

  _fb.fd = open(dev, O_RDWR);
  if (_fb.fd < 0) goto failed;
  if (ioctl(_fb.fd, FBIOGET_VSCREENINFO, &vinfo) < 0) goto failed;
  if (ioctl(_fb.fd, FBIOGET_FSCREENINFO, &finfo) < 0) goto failed;

  fcntl(_fb.fd, F_SETFD, fcntl(_fb.fd, F_GETFD) | FD_CLOEXEC);

  _fb.xoffset = vinfo.xoffset;
  _fb.yoffset = vinfo.yoffset;
  _fb.bpp = (vinfo.bits_per_pixel + 7) >> 3;
  _fb.w = vinfo.xres;
  _fb.h = vinfo.yres;
  _fb.llen = finfo.line_length;
  /* length in bytes */
  _fb.len = finfo.line_length * vinfo.yres_virtual;

  _fb.mem = mmap(NULL, _fb.len, PROT_READ | PROT_WRITE, MAP_SHARED, _fb.fd, 0);
  if (_fb.mem == MAP_FAILED) goto failed;

  /*
  --> Debug: _fb
  fprintf(stderr, "[ _fb.bpp: %d ]\n", _fb.bpp);
  fprintf(stderr, "[ _fb.w: %d ]\n", _fb.w);
  fprintf(stderr, "[ _fb.h: %d ]\n", _fb.h);
  fprintf(stderr, "[ _fb.llen %d ]\n", _fb.llen);
  fprintf(stderr, "[ _fb.len %d ]\n", _fb.len);
  --> Debug
  */

  /* initialize FB colors */
  _fb.nr = 1 << vinfo.red.length;
  _fb.ng = 1 << vinfo.green.length;
  _fb.nb = 1 << vinfo.blue.length;
  /* color shifts */
  _fb.rr = 8 - vinfo.red.length;
  _fb.rl = vinfo.red.offset;
  _fb.gr = 8 - vinfo.green.length;
  _fb.gl = vinfo.green.offset;
  _fb.br = 8 - vinfo.blue.length;
  _fb.bl = vinfo.blue.offset;

  return 0;

failed:
  fprintf(stderr,"fbInit()\n");
  close(_fb.fd);
  return 1;
}

void fbFree()
{
  munmap(_fb.mem, _fb.len);
  /*
  --> Debug: _fb
  fprintf(stderr, "framebuffer unmapped\n");
  --> Debug
  */
  close(_fb.fd);
}

u32_t fbW()
{
  return _fb.w;
}

u32_t fbH()
{
  return _fb.h;
}

void *fbRow(int r)
{
  return _fb.mem + (r + _fb.yoffset) * _fb.llen + _fb.xoffset * _fb.bpp;
}

void fbSet(int r, int c, void *mem, int len)
{
  memcpy(fbRow(r) + c * _fb.bpp, mem, len * _fb.bpp);
}

void fbBox(int sr, int er, int sc, int ec, fb32_t c)
{
  static fb32_t line[CELLW];   /* pixel line */
  int i;

  for (i = sc; i < ec; i++) {
    line[i - sc] = c;
  }

  for (i = sr; i < er; i++) {
    fbSet(i, sc, line, ec - sc);
  }
}

fb32_t fbColor(int r, int g, int b)
{
  return ((r >> _fb.rr) << _fb.rl) |
         ((g >> _fb.gr) << _fb.gl) |
         ((b >> _fb.br) << _fb.bl);
}

fb32_t fbMixedColor(fb32_t fg, fb32_t bg, fb32_t c)
{
  u8_t r = COLORMIX(R(fg), R(bg), c);
  u8_t g = COLORMIX(G(fg), G(bg), c);
  u8_t b = COLORMIX(B(fg), B(bg), c);

  return fbColor(r, g, b);
}
