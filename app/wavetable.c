/* wavetable.c
 * A simple wavetable player
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
#include <string.h>
#include <math.h>
#include <audiofile.h>

#include "util.h"
#include "wavetable.h"
#include "wavetablenote.h"

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

static void
wavetable_noteon (Machine *mach, int notenum, int vel)
{
  WaveTable *wavetable = WAVETABLE (mach);

  WaveTableNote *note = wavetable_note_new (wavetable, notenum, vel);

  g_tree_insert (wavetable->notes, GINT_TO_POINTER (notenum), note);
}

static void
wavetable_noteoff (Machine *mach, int notenum)
{
  WaveTable *wavetable = WAVETABLE (mach);

  /* TODO: wavetablenote doesn't get freed */
  g_tree_remove (wavetable->notes, GINT_TO_POINTER (notenum));
}

static void
wavetable_all_notes_off (Machine *mach)
{
  WaveTable *wavetable = WAVETABLE (mach);

  /* TODO: this is not nice */
  g_tree_destroy (wavetable->notes);
  wavetable->notes = g_tree_new (note_compare_func);
}

static gint
do_stuff_traverse_func (gpointer key, gpointer note, gpointer userdata)
{
  DoStuffTraverseData *stuff = (DoStuffTraverseData*) userdata;
  wavetable_note_do_stuff (WAVETABLE_NOTE (note),
			   stuff->buffer, stuff->mix);
  return FALSE;
}

static void
wavetable_do_stuff (Machine *mach)
{
  DoStuffTraverseData stuff;
  WaveTable *wavetable = WAVETABLE (mach);

  stuff.buffer = &(mach->out);

  /*  if ((wavegen->notes_count) > 0)
    stuff.mix = 1.0 / (wavegen->notes_count);
  else
    stuff.mix = 0.0;*/
  
  stuff.mix = 0.4;

  machine_clear_buffer (mach);

  g_tree_traverse (wavetable->notes, do_stuff_traverse_func,
		   G_IN_ORDER, &stuff);

}

static gint
free_note_traverse_func (gpointer key, gpointer note, gpointer data)
{
  wavetable_note_free (WAVETABLE_NOTE (note));
  return FALSE;
}

static void
wavetable_free (Machine *mach)
{
  WaveTable *wavetable = WAVETABLE (mach);

  /* remove all running notes
   */

  g_tree_traverse (wavetable->notes, free_note_traverse_func,
		   G_IN_ORDER, NULL);
  
  g_tree_destroy (wavetable->notes);

  machine_free (mach);
}

static void
wavetable_init_from_file (Machine *mach, gchar *filename, gint base_note)
{
  WaveTable *wavetable = WAVETABLE (mach);

  AFfilehandle affile;
  int sampleformat, width, channels;

  /* Set up callbacks
   */

  machine_init (mach);

  mach->noteon = wavetable_noteon;
  mach->noteoff = wavetable_noteoff;
  mach->do_stuff = wavetable_do_stuff;
  mach->all_notes_off = wavetable_all_notes_off;
  mach->free = wavetable_free;

  wavetable->notes = g_tree_new (note_compare_func);

  wavetable->filename = filename;

  affile = afOpenFile (filename, "r", NULL);
  if (!affile)
    {
      printf ("Can't open %s\n", filename);
      return;
    }

  wavetable->nsamples = afGetFrameCount (affile, AF_DEFAULT_TRACK);
  channels = afGetChannels (affile, AF_DEFAULT_TRACK);
  if (channels != 1)
    {
      fprintf (stderr, "only one channel files allowed\n");
      return;
    }
  wavetable->base_freq = afGetRate (affile, AF_DEFAULT_TRACK);
  afSetVirtualByteOrder (affile, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
  afGetSampleFormat (affile, AF_DEFAULT_TRACK, &sampleformat, &width);

  if (width != 16 || sampleformat != AF_SAMPFMT_TWOSCOMP)
    {
      fprintf (stderr, "I don't support this format\n");
      return;
    }

  wavetable->bits = 16;

  wavetable->samples = g_new (gint16, wavetable->nsamples);

  if (afReadFrames (affile, AF_DEFAULT_TRACK,
		    wavetable->samples, wavetable->nsamples)
      != wavetable->nsamples)
    {
      fprintf (stderr, "it didn't read all the samples\n");
      return;
    }

  afCloseFile (affile);

  wavetable->length = (wavetable->nsamples) << ACCURACY;

  wavetable->base_note = base_note;
  wavetable->base_note_freq = note_frequency (wavetable->base_note);

  printf ("loaded %s, samples: %d\n", filename, wavetable->nsamples);

}

Machine *
wavetable_new_from_file (gchar *filename, gint base_note)
{
  WaveTable *wavetable = g_new0 (WaveTable, 1);

  wavetable_init_from_file (MACHINE (wavetable), filename, base_note);

  return (MACHINE (wavetable));
}

Machine 
*wavetable_new_from_file_looped (gchar *filename, gint base_note,
				 gint start, gint length)
{
  WaveTable *wavetable = g_new0 (WaveTable, 1);

  wavetable_init_from_file (MACHINE (wavetable), filename, base_note);

  wavetable->looped = TRUE;
  wavetable->loop_start = start << ACCURACY;
  wavetable->loop_length = length << ACCURACY;
  wavetable->loop_end = (start + length - 1) << ACCURACY;

  if (wavetable->loop_end >= wavetable->length)
    {
      printf ("WARNING!!!\n");
    }

  return (MACHINE (wavetable));
}
