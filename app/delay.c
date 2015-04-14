/* delay.c - a digital delay for machines
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

#include <config.h>
#include <stdio.h>

#include "delay.h"

static void
delay_do_stuff (Machine *mach)
{
  Delay *delay = DELAY (mach);
  Machine *child = PARENT (mach)->child;

  int i;

  parent_machine_do_stuff (mach);

  if (child)
    {

      for (i=0; i < G_buf_size; i++)
	{
	  delay->buf[delay->i] = child->out.data[i] +
	    (delay->buf[(delay->i +1) % delay->n]) / delay->divisor;
      
	  mach->out.data[i] = delay->buf[delay->i];

	  delay->i = (delay->i + 1) % delay->n;
	}
    }

  mach->out.done = TRUE;
}

static void
delay_free (Machine *mach)
{
  Delay *delay = DELAY (mach);

  if (delay->buf)
    g_free (delay->buf);

  machine_free (mach);
}

Machine *
delay_new (double length, int divisor)
{
  Delay *delay  = g_new0 (Delay, 1);
  Machine *mach = MACHINE (delay);

  parent_machine_init (mach);

  mach->do_stuff = delay_do_stuff;
  mach->free = delay_free;

  delay->divisor = divisor;
  
  delay->n = (gint)(length * (double)G_sample_rate);
  delay->buf = g_new0 (MSample, delay->n);
  delay->i = 0;
  
  return mach;
}
