/* wavegennote.h - part of machines
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

#define WAVEGEN_NOTE(x)		((WaveGenNote*)(x))

typedef struct _WaveGenNote	WaveGenNote;

struct _WaveGenNote
{
  WaveGen *wavegen;	/* the wavegen that generated this note */

  int notenum;		/* MIDI note number */
  int vel;		/* MIDI velocity */

  int freq;		/* base frequency of note */
  int amplitude;	/* to multiply each sample with to get right
			 * velocity, 0 to 32767 */

  float angle;
};

WaveGenNote *wavegen_note_new (WaveGen *, int, int);
void wavegen_note_free (WaveGenNote *note);
void wavegen_note_do_stuff (WaveGenNote *note, Buffer *, float);
