/* disk.c - part of machines
 * Disk is a machine that saves it's output to disk!
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
#include <errno.h>
#include <unistd.h>

#include "machines.h"
#include "disk.h"

static void
disk_do_stuff (Machine *mach)
{
  Disk *disk = DISK (mach);
  Machine *child = PARENT (mach)->child;

  parent_machine_do_stuff (mach);

  if (child)
    {
      if (disk->file)
	{
	  fwrite (child->out.data, sizeof (MSample), G_buf_size,
		  disk->file);
	}
    }
}

static void
disk_free (Machine *mach)
{
  Disk *disk = DISK (mach);

  if (disk->file)
    {
      fclose (disk->file);
    }

  machine_free (mach);
}

Machine *
disk_new (gchar *filename)
{
  Disk *disk = NULL;
  Machine *mach = NULL;

  disk = g_new (Disk, 1);
  mach = MACHINE (disk);

  parent_machine_init (mach);

  mach->do_stuff = disk_do_stuff;
  mach->free = disk_free;

  disk->file = fopen (filename, "w");

  if (!disk->file)
    {
      g_message ("%s: %s: %s\n",
		 g_get_prgname (), filename, g_strerror (errno));
    }

  return mach;
}

