/*
  conf.h 

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#ifndef _CONF_H_
#define _CONF_H_

/* fonts */
#define SFONT   "/usr/share/fonts/TTF/DejaVuSansMono.ttf"
#define DFONT   "/usr/share/fonts/TTF/DroidSansFallbackFull.ttf"
#define SBFONT  "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf"

#define FONTPXH 15        /* font pixel height */
#define XLSPACE 0         /* extra line space in pixels */
#define FORCEHINT 0       /* force freetype to use autohint */

/* text (character) screen definition */
#define NCOLS 256         /* maximum number of columns of the screen buffer */
#define NROWS 128         /* maximum number of rows of the screen buffer */
#define CELLH 25          /* maximum cell height in pixels of a character */
#define CELLW 25          /* maximum cell width in pixels of a character */

/* font specific masks */
#define FNTB 0x01000000   /* bold font mask */
#define FNTI 0x02000000   /* italic font mask */
#define FNTC 0x00ffffff   /* font color mask */

#define U32_NCHARS 1024   /* UTF-32: maximum u32_t characters */
#define U16_NCHARS 1024   /* UTF-16: maximum u16_t characters */
#define U8_NCHARS 4096    /* UTF-8: size: u32_t = 4 * s8_t */

/* black */
#define COLOR0  0x000000
#define COLOR8  0x555555
/* red */
#define COLOR1  0xaa0000
#define COLOR9  0xff5555
/* green */
#define COLOR2  0x00aa00
#define COLORA  0x55ff55
/* yellow */
#define COLOR3  0xaa5500
#define COLORB  0xffff55
/* blue */
#define COLOR4  0x0000aa
#define COLORC  0x5555ff
/* magenta */
#define COLOR5  0xaa00aa
#define COLORD  0xff55ff
/* cyan */
#define COLOR6  0x00aaaa
#define COLORE  0x55ffff
/* white */
#define COLOR7  0xaaaaaa
#define COLORF  0xffffff

/* some math macros */
#define MIN(a, b)  (a < b ? a : b)
#define MAX(a, b)  (a > b ? a : b)
#define LEN(a)  (sizeof(a) / sizeof((a)[0]))

/* linefeed */
#define LF  0x000a
#define CR  0x000d


#endif
