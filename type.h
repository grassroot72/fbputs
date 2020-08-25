/*
 * Copyright (C) 2019  Edward LEI <edward_lei72@hotmail.com>
 *
 * The code is licensed under the MIT license
 */

#ifndef _TYPE_H_
#define _TYPE_H_


/* fb32_t: describes framebuffer value 0xAARRGGBB */
typedef unsigned int fb32_t;

/* ft32_t: describes the font type (bold, italic) and font color */
typedef unsigned int ft32_t;

/* types: for identifying bit-width (ex. unicode character width)  */
typedef unsigned int u32_t;      /* char32_t */
typedef unsigned short u16_t;    /* char16_t */
typedef char s8_t;
typedef unsigned char u8_t;


#endif
