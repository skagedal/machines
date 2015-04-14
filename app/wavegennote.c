/* wavegennote.c - part of machines
 * A simple wave generator (or oscillator)
 *
 * Copyright (C) 1998 Simon Kågedal
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
#include <glib.h>
#include <math.h>
#include "util.h"
#include "machines.h"
#include "wavegen.h"
#include "wavegennote.h"

/* create a note */

WaveGenNote *
wavegen_note_new (WaveGen *wavegen, int notenum, int vel)
{
  WaveGenNote *note = g_new0 (WaveGenNote, 1);

  note->wavegen = wavegen;
  note->notenum = notenum;
  note->vel = vel;

  /* stuff we want to calculate now */
  note->freq = note_frequency (notenum);
  note->amplitude = (((float)vel) / 127.0) * 32767;
  note->angle = 0.0;

  g_print ("freq: %i\tamp: %i\n", note->freq, note->amplitude);
  g_print ("sample rate: %i\n", G_sample_rate);
  return note;
}

/* free a note */

void
wavegen_note_free (WaveGenNote *note)
{
  g_free (note);
}

/* add this note's data to the mix */

void
wavegen_note_do_stuff (WaveGenNote *note, Buffer *buffer, float mix)
{
  int i;

  float inc_angle;

  inc_angle = (2.0 * M_PI) / (G_sample_rate / note->freq);

  for (i=0; i < G_buf_size; i++)
    {
      switch (note->wavegen->type)
	{
	case WAVEGEN_SQUARE:
	  if (note->angle < M_PI)
	    buffer->data[i] -= (note->amplitude * mix);
	  else
	    buffer->data[i] += (note->amplitude * mix);

	  break;
	  
	case WAVEGEN_SAWTOOTH:
	  buffer->data[i] += (note->amplitude * mix 
			      * ((note->angle / (2.0 * M_PI)) * 2.0 - 1.0));
	  break;

	case WAVEGEN_SINE:
	   buffer->data[i] += (note->amplitude * mix 
			       * sin (note->angle));
	  break;
	}

      note->angle += inc_angle;
      if (note->angle > (2.0 * M_PI))
	note->angle -= (2.0 * M_PI);
	  
    }
}






