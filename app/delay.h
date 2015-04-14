/* delay.h - a digital delay for machines
 *
 * Copyright (C) 1999 Simon Kågedal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License in the file COPYING for more details.
 */

#include "machines.h"

#define DELAY(mach)	        ((Delay*)(mach))

typedef struct _Delay		Delay;

struct _Delay
{
  ParentMachine mach;			/* inheritance */

  gint divisor;			/* by which data is divided every loop */

  MSample *buf;			/* delay buffer */
  gint n;			/* size of buf */
  gint i;			/* current pointer in buf */
};


Machine *delay_new (double length, int divisor);
