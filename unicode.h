/*
 * Copyright (C) 2019  Edward LEI <edward_lei72@hotmail.com>
 *
 * The code is licensed under the MIT license
 */

#ifndef _UNICODE_H_
#define _UNICODE_H_

#include "type.h"


/*
 * Check if a character is a double-width character
 */
int ucIsDW(u16_t c);

/*
 * Load a buffer from a UTF-16 encoded file
 *
 * Return: length in u16_t characters (BOM excluded)
 *         0 - error occurred
 */
u32_t ucLoadToU16(u16_t *u16buf, char *fname);


#endif
