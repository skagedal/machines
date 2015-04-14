/* mixer.h - part of machines
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

#define MIXER(x)		((Mixer*)(x))

typedef struct _MixMachine	MixMachine;
typedef struct _Mixer		Mixer;

/* this struct holds info about how one machine shoul be mixed */

struct _MixMachine
{
  Machine *mach;
  double mix;
};

struct _Mixer
{
  Machine mach;			/* inheritance */

  GSList *mixmachines;		/* list of (MixMachine *) */
};

Machine *mixer_new ();
void mixer_add_child (Machine *mach, Machine *child, double mix);


