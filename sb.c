/*
  sb.c

  Copyright (C) 2019 by Edward LEI

  This code is licensed under the MIT license
*/

#include "conf.h"
#include "fb.h"
#include "font.h"
#include "unicode.h"
#include "sb.h"


/*----- Internally used variables & functions --------------------------------*/

struct _screenbuffer_s {
  /*
    The terminal screen is filled with characters. A single width character
    takes a single cell whilst a double width character takes 2 consecutive
    cells
  */
  int row;      /* current cell row number */
  int col;      /* current cell column number */

  /* visible cells */
  int nrows;    /* maximum number of visible screen rows */
  int ncols;    /* maximum number of visible screen cols (cells) */

  u16_t buf[NROWS * NCOLS];     /* screen buffer */
  ft32_t fgs[NROWS * NCOLS];    /* character foreground color */
  fb32_t bgs[NROWS * NCOLS];    /* character background color */
};

/* Note: I know it is not a good practice to declare avariable _sb at this
         scope and this makes the code quite cohesive, but I couldn't refuse
         the following benefits,
   (a). convenient to protect struct _screenbuffer_s's data members
   (b). shorter function code (if you believe ... :)
*/
static struct _screenbuffer_s _sb;

/*----- End: Internally used variables & functions ---------------------------*/


void sbInit()
{
  _sb.ncols = fbW() / ftW();
  _sb.nrows = fbH() / ftH();

  _sb.col = 0;
  _sb.row = 0;
}

int sbNCols()
{
  return _sb.ncols;
}

int sbNRows()
{
  return _sb.nrows;
}

int sbCol()
{
  return _sb.col;
}

int sbRow()
{
  return _sb.row;
}

void sbPutc(u16_t ch, int dw, ft32_t fg, ft32_t bg)
{
  ftCh2FB(_sb.row, _sb.col, ch, dw, fg & FNTB, fg & FNTC, bg);
  if (dw) {
    _sb.col++;
  }
  _sb.col++;
}

void sbPuts(u16_t *txt, ft32_t fg, ft32_t bg)
{
  u16_t ch;
  u16_t *u16p;   /* pointer to the UTF-16 buffer */
  int dw;        /* double width character? */

  u16p = txt;

  while (*u16p) {
    ch = *u16p;
    if (ch != LF) {    /* not LF */
      if (ch == CR) {  /* CR */
        u16p++;
      }
      else {
        if (_sb.col < _sb.ncols) {
          dw = ucIsDW(ch);
          sbPutc(ch, dw, fg, bg);

          u16p++;
        }
        else {         /* soft LF */
          _sb.row++;
          _sb.col = 0;
        }
      }
    }
    else {             /* LF */
      _sb.row++;
      _sb.col = 0;
      u16p++;
    }
  }
}