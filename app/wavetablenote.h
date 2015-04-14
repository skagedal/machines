/* wavetablenote.h - part of machines
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

#define WAVETABLE_NOTE(x)		((WaveTableNote*)(x))

typedef struct _WaveTableNote WaveTableNote;

struct _WaveTableNote
{
  WaveTable *wavetable;		/* daddy */

  int notenum;		/* MIDI note number */
  int vel;		/* MIDI velocity */

  gboolean is_playing;
  int isamples;			/* number of samples played */
  int inc;			/* increase per sample */

  float amplitude;		/* to multiply each sample with to get right
				 * velocity, 0.0 to 1.1 */
  
};

WaveTableNote *wavetable_note_new (WaveTable *, int, int);
void wavetable_note_free (WaveTableNote *note);
void wavetable_note_do_stuff (WaveTableNote *note, Buffer *, float);
