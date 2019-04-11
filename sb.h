/*
  sb.h

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#ifndef _SB_H_
#define _SB_H_

#include "type.h"


/*
  Initialize screenbuffer
*/
void sbInit();

/*
  Return: the number of character columns of the visible screen
*/
int sbNCols();

/*
  Return：the number of character rows of the visible screen
*/
int sbNRows();

/*
  Return: the current character column number
*/
int sbCol();

/*
  Return: the current character row number
*/
int sbRow();

/* 
  Emulates the putc function
    ch - character to be put in framebuffter
    dw - double width character?
    fg - font type(bold) | font foreground color
    bg - background color
*/
void sbPutc(u16_t ch, int dw, ft32_t fg, ft32_t bg);

/* 
  Emulates the puts function
    txt - text to be printed
    fg - font type(bold) | font foreground color
    bg - background color
*/
void sbPuts(u16_t *txt, ft32_t fg, ft32_t bg);


#endif