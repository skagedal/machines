/* disk.h - part of machines
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

#include "machines.h"

#define DISK(x)		((Disk*)(x))

typedef struct _Disk		Disk;

struct _Disk
{
  ParentMachine mach;		/* inheritance */

  FILE *file;
};

Machine *disk_new (gchar *);
