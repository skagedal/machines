/* wavetablenote.c - part of machines
 * A simple wavetable player
 * I don't know...  Just fooling around
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
#include "wavetable.h"
#include "wavetablenote.h"

WaveTableNote *
wavetable_note_new (WaveTable *wavetable, int notenum, int vel)
{
  WaveTableNote *note = g_new(WaveTableNote, 1);

  note->wavetable = wavetable;
  note->notenum = notenum;
  note->vel = vel;
  note->amplitude = (float)vel / 127.0;
  
  note->is_playing = TRUE;
  note->isamples = 0;

  note->inc = (wavetable->base_freq * note_frequency (notenum) /
	       wavetable->base_note_freq);
  note->inc = (note->inc << ACCURACY) / G_sample_rate;

  return note;
}

void
wavetable_note_free (WaveTableNote *note)
{
  g_free (note);
}


/* add this note's data to the mix */

void
wavetable_note_do_stuff (WaveTableNote *note, Buffer *buffer, float mix)
{
  if (note->is_playing)
    {
      gint16 *samples = note->wavetable->samples;
      int i, sampi;

      for (i=0; i < G_buf_size; i++)
	{
	  if (note->wavetable->looped)
	    {
	      if (note->isamples >= (note->wavetable->loop_end))
		{
		  note->isamples = 
		    note->wavetable->loop_start;
		}
	    }
	  else
	    {
	      if (note->isamples >= (note->wavetable->length-1))
		{
		  note->is_playing = FALSE;
		  break;
		}
	    }

	  sampi = (note->isamples) >> ACCURACY;

	  buffer->data[i] += mix *
	    (samples[sampi]
#ifdef INTERPOLATION
	     + 
	     (samples[sampi+1] - samples[sampi]) *
	     (note->isamples % (1<<ACCURACY)) / (1<<ACCURACY)
#endif
);

	  /*
	  buffer->data[i] += mix *
	    note->wavetable->samples[(note->isamples) >> ACCURACY];
	    */

	  note->isamples += note->inc;
	}
    }
}


