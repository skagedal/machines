/* wavegen.c - part of machines
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
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "util.h"
#include "wavegen.h"
#include "wavegennote.h"

typedef struct _DoStuffTraverseData DoStuffTraverseData;

struct _DoStuffTraverseData
{
  Buffer *buffer;
  float mix;
};

static gint
note_compare_func (gconstpointer a, gconstpointer b)
{
  return (GPOINTER_TO_INT(a) - GPOINTER_TO_INT(b));
}

void
wavegen_noteon(Machine *mach, int notenum, int vel)
{
  WaveGen *wavegen = WAVEGEN (mach);

  WaveGenNote *note = wavegen_note_new (wavegen, notenum, vel);
  
  g_tree_insert (wavegen->notes, GINT_TO_POINTER (notenum), note);
}

void
wavegen_noteoff (Machine *mach, int notenum)
{
  WaveGen *wavegen = WAVEGEN (mach);

  /* TODO: BAD STUFF: doesn't free the WaveGenNote! */
  g_tree_remove (wavegen->notes, GINT_TO_POINTER (notenum));
}

void
wavegen_all_notes_off (Machine *mach)
{
  WaveGen *wavegen = WAVEGEN (mach);

  /* BAD STUFF: doesn't free the WaveGenNote! */
  /* I know this (below) sucks, but I'm gonna switch to another
   * tree implementation (libavl) */
  g_tree_destroy (wavegen->notes);
  wavegen->notes = g_tree_new (note_compare_func);
}

static gint
do_stuff_traverse_func (gpointer key, gpointer note, gpointer userdata)
{
  DoStuffTraverseData *stuff = (DoStuffTraverseData*) userdata;
  wavegen_note_do_stuff (WAVEGEN_NOTE (note), 
			 stuff->buffer, stuff->mix);

  return FALSE;
}

void
wavegen_do_stuff(Machine *mach)
{
  DoStuffTraverseData stuff;
  WaveGen *wavegen = WAVEGEN (mach);

  machine_clear_buffer (mach);

  /* TODO: this is kind of not needed anymore since the notes
   * know their parents */
  stuff.buffer = &(mach->out);
  stuff.mix = 0.1;

  g_tree_traverse (wavegen->notes, do_stuff_traverse_func, 
		   G_IN_ORDER, &stuff);
}

static gint
free_note_traverse_func (gpointer key, gpointer note, gpointer data)
{
  wavegen_note_free (WAVEGEN_NOTE (note));
  return FALSE;
}

void
wavegen_free(Machine *mach)
{
  WaveGen *wavegen = WAVEGEN (mach);

  /* remove all running notes
   */

  g_tree_traverse (wavegen->notes, free_note_traverse_func,
		   G_IN_ORDER, NULL);

  g_tree_destroy (wavegen->notes);

  machine_free (mach);
}

/* initializes (fills) a sine wavetable */
/* TODO: move to WaveTable instrument */

/*
void
init_wavetable_sine (gfloat *wavetable, gint size)
{
  gfloat increment = (2.0 * M_PI) / size;
  int i;
  for (i=0; i<size; i++)
    {
      wavetable[i] = sin (i * increment);
    }
}
*/

/* initializes a wavegen */

static void
wavegen_init(Machine *mach, WaveGenType type)
{
  WaveGen *wavegen = WAVEGEN (mach);

  machine_init (mach);

  /* Set up callbacks
   */

  mach->noteon = wavegen_noteon;
  mach->noteoff = wavegen_noteoff;
  mach->do_stuff = wavegen_do_stuff;
  mach->all_notes_off = wavegen_all_notes_off;
  mach->free = wavegen_free;

  wavegen->notes = g_tree_new (note_compare_func);

  wavegen->type = type;
#ifdef 0
  if (type == WAVEGEN_SINE)
    {
      wavegen->wavetable_size = 1024;
      wavegen->wavetable = g_new (gfloat, wavegen->wavetable_size);
      init_wavetable_sine (wavegen->wavetable, wavegen->wavetable_size);
    }
  else
    {
      wavegen->wavetable_size = 0;
      wavegen->wavetable = NULL;
    }
#endif
}

/* `wavegen_new' creates a new wave generator
 */

Machine *
wavegen_new (WaveGenType type)
{
  WaveGen *wavegen = g_new (WaveGen, 1);

  wavegen_init (MACHINE (wavegen), type);

  return (MACHINE (wavegen));
}

