/* machines.c - part of machines
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
#include <glib.h>

#include <errno.h>
#include <unistd.h>		/* for `write' */
#include <fcntl.h>

#include "midievents.h"
#include "midi.h"
#include "machines.h"
#include "audio_oss.h"

/* machines */
#include "speaker.h"
#include "mixer.h"
#include "wavegen.h"
#include "wavetable.h"
#include "delay.h"
#include "flanger.h"

#include "lipaconf.h"

int G_sample_rate = 44100;
int G_buf_size = 1024;

static int midi_fd = 0;		/* just standard input (for now) */

static GSList *mach_list = NULL;
static GSList *top_list = NULL;
static MidiHandle *midihandle;

/* Defines what machines listen to the MIDI channels
 * A GSList of (Machine *) 
 */
static GSList *channelmach[16];

/* `machines_midi_handle' is as callback that gets called when
 * a MIDI event comes
 */

static void 
machines_midi_handler (MidiHandle *handle)
{
  int note, vel;

  int chan = MIDI_GET_CHANNEL (handle);
  GSList *cur;
  Machine *mach;

  /* = channelmach[chan]; */
 
  for (cur = channelmach[chan]; cur; cur = cur->next)
    {
      mach = MACHINE (cur->data);

      switch (MIDI_GET_STATUS (handle))
	{
	case MIDI_NOTE_OFF:
	  note = MIDI_GET_NOTE (handle);
	  mach->noteoff (mach, note);
	  break;
	  
	case MIDI_NOTE_ON:
	  note = MIDI_GET_NOTE (handle);
	  vel = MIDI_GET_VELOCITY (handle);
	  if (vel == 0)
	    mach->noteoff (mach, note);
	  else
	    mach->noteon (mach, note, vel);
	  break;

	case MIDI_CTRL_CHANGE:
	  if (mach->all_notes_off)
	    mach->all_notes_off (mach);
	  break;
	}
    }
}

/* this create some test machines */

void
test_init (void)
{
  int i;

  Machine *sine = wavegen_new (WAVEGEN_SINE);
  Machine *bs = wavetable_new_from_file ("bs.wav", 60);
  Machine *strings = wavetable_new_from_file_looped ("strings.wav", 60,
						     0x11d9, 0x352e);
  Machine *mixer = mixer_new ();
  Machine *out = speaker_new ();

  Machine *delay = delay_new (0.25, 2);
  Machine *flanger = flanger_new (2.0, 0.00453, 0.0011, 2.0,
				  SWEEP_TRIANGLE, 1.0);

  /* every machine gets added to this list */

  machines_register (flanger);
  machines_register (delay);
  machines_register (sine);
  machines_register (bs);
  machines_register (strings);
  machines_register (mixer);
  machines_register (out);

  /* define the relations */
  
  parent_machine_set_child (delay, bs);
  parent_machine_set_child (flanger, sine);

  mixer_add_child (mixer, delay, 0.5);
  mixer_add_child (mixer, flanger, 0.5);
  mixer_add_child (mixer, strings, 0.5);
  parent_machine_set_child (out, mixer);
  machines_add_top_node (out);

  /* channel setup */

  machines_add_to_midi_channel (bs, 0);
  machines_add_to_midi_channel (strings, 1);

  for (i=2; i<16; i++)
    {
      machines_add_to_midi_channel (sine, i);
    }

}

/* `machines_init': create and initialize all machines
 */

gboolean
machines_init (gchar *filename)
{
  gint i;

  for (i=0; i<16; i++)
    {
      channelmach[i] = NULL;
    }

  /*
  test_init ();
  */

  lipaconf_read_file (filename);

  /* MIDI */

  midihandle = midi_handle_new (machines_midi_handler);

  /* set non-block on midi fd */

  if (fcntl (midi_fd, F_SETFL, O_NONBLOCK) == -1)
    {
      fprintf (stderr, "%s: %s\n", 
	       g_get_prgname (), g_strerror (errno));
      return FALSE;
    }
  else
    {
      return TRUE;
    }
}

/* register a machine */

void
machines_register (Machine *mach)
{
  mach_list = g_slist_prepend (mach_list, mach);
}

/* add machine to the list of "top" machines */

void
machines_add_top_node (Machine *mach)
{
  top_list = g_slist_prepend (top_list, mach);
}

/* make a machine listen to a MIDI channel */

void
machines_add_to_midi_channel (Machine *mach, gint channel)
{
  g_return_if_fail (channel >= 0 && channel <= 15);

  channelmach [channel] = g_slist_prepend (channelmach [channel], mach);
}

/* if there's MIDI data, read it */

static void
readmidi ()
{
  guchar b;
  ssize_t n;

  /* would be better to read several bytes at once... */

  do
    {
      n = read (midi_fd, &b, 1);

      if (n == 1)
	{
	  midi_add_byte (midihandle, b);
	}
      else if (n == -1 && errno != EAGAIN)
	{
	  fprintf (stderr, "%s: %s\n",
		   g_get_prgname (), g_strerror (errno));
	  break;
	}
    }
  while (n == 1);
}

/* the main loop of the app */

void
machines_loop (void)
{
  while (1)
    {
      GSList *cur;
      Machine *mach;

      readmidi ();

      /* unmark every machine */

      for (cur = mach_list; cur; cur = cur->next)
	{
	  mach = cur->data;
	  mach->out.done = FALSE;
	}

      /* start at every top node */

      for (cur = top_list; cur; cur = cur->next)
	{
	  mach = cur->data;
	  mach->do_stuff (mach);
	}

    }

  /* TODO: make sure to free everything */

  /*  audio_oss_loop (); */
}

void
machines_free (void)
{
  GSList *list = NULL;

  /* free all machines */

  for (list=mach_list; list; list=list->next)
    {
      Machine *mach = MACHINE (list->data);
      mach->free (mach);
    }

  /* free machine list */

  g_slist_free (mach_list);
  mach_list = NULL;
  g_slist_free (top_list);
  top_list = NULL;
}

/***********************************************************************
 * Functions operating on one Machine
 ***********************************************************************/

void
machine_init (Machine *mach)
{
  mach->noteon = NULL;
  mach->noteoff = NULL;
  mach->do_stuff = NULL;
  mach->all_notes_off = NULL;
  mach->free = machine_free;

  mach->out.data = (MSample*) g_new0 (MSample, 
				      G_buf_size);
}

void
machine_free (Machine *mach)
{
  g_free (mach);
}

/***********************************************************************
 * Functions operating on one ParentMachine
 ***********************************************************************/

void
parent_machine_init (Machine *mach)
{
  machine_init (mach);

  PARENT (mach)->child = NULL;
}

void
parent_machine_set_child (Machine *mach, Machine *child)
{
  PARENT (mach)->child = child;
}

void
parent_machine_do_stuff (Machine *mach)
{
  Machine *child = PARENT (mach)->child;

  if (child)
    {
      if (! child->out.done)
	{
	  child->do_stuff (child);
	}
    }
}
