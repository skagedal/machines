/* mixer.c - part of machines
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

#include "mixer.h"

static void
mixer_do_stuff (Machine *mach)
{
  Mixer *mixer = MIXER (mach);
  GSList *cur;

  int i;

  machine_clear_buffer (mach);
  
  /* for every child... */

  for (cur = mixer->mixmachines; cur; cur = cur->next)
    {
      MixMachine *mixmach = ((MixMachine*)(cur->data));

      /* make sure the child does it's stuff */

      if (! (mixmach->mach->out.done))
	{
	  mixmach->mach->do_stuff (mixmach->mach);
	}

      /* add all data in buffer */

      for (i=0; i < G_buf_size; i++)
	{
	  mach->out.data[i] += 
	    ((double)(mixmach->mach->out.data[i]) * mixmach->mix);
	}
    }
}

Machine *
mixer_new (void)
{
  Mixer *mixer = g_new (Mixer, 1);
  Machine *mach = MACHINE (mixer);

  machine_init (mach);

  mach->do_stuff = mixer_do_stuff;

  mixer->mixmachines = NULL;

  return mach;
}


void
mixer_add_child (Machine *mach, Machine *child, double mix)
{
  MixMachine *mixmach = g_new (MixMachine, 1);
  
  mixmach->mach = child;
  mixmach->mix = mix;

  MIXER (mach)->mixmachines =
    g_slist_prepend (MIXER (mach)->mixmachines, mixmach);
}
