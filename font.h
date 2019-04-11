/*
  font.h

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#ifndef _FONT_H_
#define _FONT_H_

#include "type.h"


/*
  Load freetype2 fonts
    sfnt  - single width font
    dfnt  - double width font 
    sbfnt - single width font (bold)
    dbfnt - double width font (bold)
    h     - font height in pixels

  Return: 0 - success
          1 - failed 
*/
int ftLoad(char *sfnt, char *dfnt, char *sbfnt, char *dbfnt, int h);

/*
  Free the font resource
*/
void ftFree();

/*
  Return: single cell height in pixels
*/
int ftH();

/*
  Return: single cell width in pixels
*/
int ftW();

/*
  Draw a character on the framebuffter
    r  - character start row number
    c  - character start column number
    ch - character to be put in framebuffter
    dw - double width character?
    b  - bold?
    fg - font foreground color
    bg - font background color
*/
void ftCh2FB(int r, int c, u16_t ch, int dw, int b, fb32_t fg, fb32_t bg);


#endif