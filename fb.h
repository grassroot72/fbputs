/*
  fb.h

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#ifndef _FB_H_
#define _FB_H_

#include "type.h"


/*
  Initialize the bramebuffer

  Return: 0 - success
          1 - failed
*/
int fbInit(char *dev);

/*
  free the framebuffer
*/
void fbFree();

/*
  Return: FB screen resolution x
*/
u32_t fbW();

/*
  Return: FB screen resolution y
*/
u32_t fbH();

/*
  Return: the start address of a pixel row on the framebuffer
*/
void *fbRow(int r);

/*
  Draw a row on the framebuffer
    r   - pixel row
    c   - pixel column
    mem - memory to be copied to framebuffer
    len - mem length
*/
void fbSet(int r, int c, void *mem, int len);

/* 
  Draw a box with color c on the framebuffer
    sr - pixel start row
    er - pixel end row
    sc - pixel start column
    ec - pixel end column
    c  - color value (in one unsigned integer)
*/
void fbBox(int sr, int er, int sc, int ec, fb32_t c);

/*
    r - [0 ~ 255]
    g - [0 ~ 255]
    b - [0 ~ 255]

  Return: the FB color
*/
fb32_t fbColor(int r, int g, int b);

/*
  Return: the mixed FB color
*/
fb32_t fbMixedColor(fb32_t fg, fb32_t bg, fb32_t c);


#endif