/* wavetable.h - part of machines
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

#include "machines.h"

#define ACCURACY (12)

#define WAVETABLE(x)	((WaveTable*)(x))

typedef struct _WaveTable	WaveTable;

struct _WaveTable
{
  Machine mach;			/* inheritance */
  
  gchar *filename;		/* filename */
  int bits;			/* typically 16 */
  int base_freq;		/* play this sound at `base_freq'... */
  int base_note;		/* ...and you get the note `base_note' */
  int base_note_freq;		/* the frequency of that note */

  int nsamples;			/* number of samples */
  int length;			/* number of samples << ACCURACY */
  gint16 *samples;		/* buffer containing wavetable */

  gboolean looped;		/* is it looped? */
  gint loop_start;		/* this is << ACCURACY */
  gint loop_length;		/* this too */
  gint loop_end;		/* this is just loop_start + loop_length - 1*/

  GTree *notes;			/* running notes (WaveTableNote)  */
};

Machine *wavetable_new_from_file (gchar *filename, gint);
Machine *wavetable_new_from_file_looped (gchar *filename, gint, gint, gint);
