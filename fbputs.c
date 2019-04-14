/*
  fbputs.c

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#include <stdio.h>

#include "type.h"
#include "conf.h"
#include "fb.h"
#include "font.h"
#include "sb.h"
#include "unicode.h"


int main(int argc, char **argv)
{
  /* get something to display */
  u16_t u16buf[U16_NCHARS];
  u32_t u16len;


  if (fbInit("/dev/fb0")) {
    fprintf(stderr, "failed to initialize the framebuffer\n");
    return 1;
  }

  if (ftLoad(SFONT, DFONT, SBFONT, 0, FONTPXH)) {
    fprintf(stderr, "font: failed to load font\n");
    return 1;
  }

  /* screenbuffer should be initialized after font is loaded */
  sbInit();


  if (argc == 2) {
    u16len = ucLoadToU16(u16buf, argv[1]);
    if (u16len != 0) {
      sbPuts(u16buf, FNTB | COLORF, COLOR0);
    }
  }
  else {
#ifdef DEMO
    /* UTF-8 encoded file , linux LF end of line */
    ucLoadToU16(u16buf, "UTF-8-LF.txt");
    sbPuts(u16buf, FNTB | COLORA, COLOR0);
    sbPutc(u16buf[4], ucIsDW(u16buf[4]), FNTB | COLOR7, COLOR1);
    /* LF, LF */
    u16buf[0] = LF;
    u16buf[1] = LF;
    u16buf[2] = 0;
    sbPuts(u16buf, FNTB | COLORF, COLOR0);


    /* UTF-16 encoded file, windows CRLF end of line */
    ucLoadToU16(u16buf, "UTF-16-CRLF.txt");
    sbPuts(u16buf, FNTB | COLOR3, COLOR0);
    sbPutc(u16buf[4], ucIsDW(u16buf[4]), FNTB | COLOR0, COLORF);
    /* LF, LF */
    u16buf[0] = LF;
    u16buf[1] = LF;
    u16buf[2] = 0;
    sbPuts(u16buf, FNTB | COLORF, COLOR0);


    /* UTF-8 encoded file, windows CRLF end of line */
    ucLoadToU16(u16buf, "UTF-8-CRLF.txt");
    sbPuts(u16buf, FNTB | COLOR6, COLOR0);
    sbPutc(u16buf[4], ucIsDW(u16buf[4]), FNTB | COLOR1, COLORB);
    /* LF, LF */
    u16buf[0] = LF;
    u16buf[1] = LF;
    u16buf[2] = 0;
    sbPuts(u16buf, FNTB | COLORF, COLOR0);


    /* UTF-16 encoded file, Linux LF end of line */
    ucLoadToU16(u16buf, "UTF-16-LF.txt");
    sbPuts(u16buf, FNTB | COLOR5, COLOR0);
    sbPutc(u16buf[4], ucIsDW(u16buf[4]), FNTB | COLORB, COLOR4);
#else
    fprintf(stderr, "You need to provide a filename\n");
#endif
  }

  ftFree();
  fbFree();

  return 0;
}
