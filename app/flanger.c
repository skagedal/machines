/* flanger.c - a digital flanger for machines
 *
 * Copyright (C) 1999 Henrik Nyman
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
#include <stdlib.h>
#include <math.h>
#include "flanger.h"

static void
flanger_do_stuff (Machine *mach)
{
  Flanger *flanger = FLANGER (mach);
  Machine *child = PARENT (mach)->child;

  float inc_angle, cur_delay, f, effect_val;
  int i, i1, i2;
  /*static long debug_i;*/

  parent_machine_do_stuff (mach);

  inc_angle = (2.0 * M_PI) / (G_sample_rate / flanger->rate);

  for (i=0; i < G_buf_size; i++)
    {
      switch (flanger->waveform)
	{
	case SWEEP_SINE:
	  cur_delay = flanger->delay + 
	              sin( flanger->angle )*flanger->sweep_depth*0.5 +
	              flanger->sweep_depth*0.5;	  
	  break;

	case SWEEP_TRIANGLE:
	  cur_delay = flanger->delay;
	  if( flanger->angle <= M_PI ) 
	      cur_delay += flanger->sweep_depth * flanger->angle / M_PI;
	  else
	      cur_delay += flanger->sweep_depth - 
		           flanger->sweep_depth * (flanger->angle-M_PI) / M_PI;
	  break;
	  
	default:
	  /* To get rid of warning... */
	  return;
	}

      /* Calculate delayed effect value using linear interpolation */
      f = cur_delay - floor(cur_delay);
      i1 = (flanger->bufsize + flanger->i - (int)floor(cur_delay)) % 
	    flanger->bufsize;
      i2 = (flanger->bufsize + flanger->i - (int)ceil(cur_delay)) % 
	    flanger->bufsize;
      effect_val = (float)(flanger->buf[ i1 ]) * (1.0-f) + 
		   (float)(flanger->buf[ i2 ]) * f;

      /* Buffer input mixed with feedback */
      flanger->buf[flanger->i] = (( child->out.data[i] + 
				    effect_val*flanger->feedback ) /
				  (1.0 + flanger->feedback)) + 0.5;

      /* Mix effect value with original */
      mach->out.data[i] = (( child->out.data[i] + 
				effect_val*flanger->mix ) /
			      (1.0 + flanger->mix)) + 0.5;

      /*
       *if( debug_i++ % 17933L == 0 ) 
       *  printf( "D:%.2f I:%+5d i1:%+5d i2:%+5d Inp:%+5d B1:%+5d B2:%+5d"
       *	  "E:%+5d O:%+5d\n", cur_delay, flanger->i, i1, i2, 
       *	  flanger->buf[flanger->i], flanger->buf[i1], flanger->buf[i2],
       *	  (int)(effect_val), bufinfo->out[i] );
       */

      flanger->i = (flanger->i + 1) % flanger->bufsize;

      flanger->angle += inc_angle;
      if (flanger->angle > (2.0 * M_PI))
	flanger->angle -= (2.0 * M_PI);
    }
}

static void
flanger_free (Machine *mach)
{
  Flanger *flanger = FLANGER (mach);

  if (flanger->buf)
    g_free (flanger->buf);

  machine_free (mach);
}

Machine *
flanger_new (float mix, float delay_length, float sweep_depth_length,
	     float rate, SweepWaveform waveform, float feedback )
{
  Flanger *flanger  = g_new0 (Flanger, 1);
  Machine *mach = MACHINE (flanger);

  parent_machine_init (mach);

  mach->do_stuff = flanger_do_stuff;
  mach->free = flanger_free;

  flanger->mix = mix;
  flanger->delay = (gint)(delay_length * (float)G_sample_rate);
  flanger->sweep_depth = (gint)(sweep_depth_length * (float)G_sample_rate);
  flanger->rate = rate;
  flanger->waveform = waveform;
  flanger->feedback = feedback;
  flanger->bufsize = flanger->delay + flanger->sweep_depth + 1;
  flanger->buf = g_new0 (MSample, flanger->bufsize );
  flanger->i = 0;
  flanger->angle = 0.0;
  
  return mach;
}


