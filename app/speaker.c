/* speaker.c - part of machines
 * Speaker is the machine that makes sound
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
#include <unistd.h>

#include "machines.h"
#include "speaker.h"

extern int audio_fd;

static void
speaker_do_stuff (Machine *mach)
{
  Machine *child = PARENT (mach)->child;

  /* no child, no sound */

  parent_machine_do_stuff (mach);

  if (child)
    {
      write (audio_fd, child->out.data, 
	     sizeof(MSample) * G_buf_size);
    }
}

Machine *
speaker_new (void)
{
  /* to prevent the user from creating several Speaker instruments
   * (at least right now that doesn't make sense) 
   */

  static gboolean one_output = FALSE;

  Speaker *speaker = NULL;
  Machine *mach = NULL;

  if (!one_output)
    {
      speaker = g_new (Speaker, 1);
      mach = MACHINE (speaker);

      parent_machine_init (mach);

      mach->do_stuff = speaker_do_stuff;

      one_output = TRUE;
    }
  else
    {
      g_message ("You can only have one Speaker machine");
    }

  return mach;
}



