/* machines.h - part of machines
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

#ifndef _MACHINES_H_
#define _MACHINES_H_

#include <glib.h>
#include <string.h>

#define MACHINE(x)		((Machine*)(x))
#define PARENT(x)		((ParentMachine*)(x))

typedef gint16			MSample;

typedef struct _Machine 	Machine;
typedef struct _ParentMachine	ParentMachine;
typedef struct _Buffer	 	Buffer;

struct _Buffer
{
  gboolean done;		/* if the buffer is filled this round */
  MSample *data;
};

struct _Machine
{
  void (*noteon) (Machine *, int note, int amp);
  void (*noteoff) (Machine *, int note);

  void (*do_stuff) (Machine *);
  void (*all_notes_off) (Machine *);

  void (*free) (Machine *);

  Buffer out;
};

/* this kind of machine has one child */

struct _ParentMachine
{
  Machine mach;

  Machine *child;
};

gboolean machines_init (gchar *filename);
void machines_free (void);
void machines_loop (void);

void machines_register (Machine *mach);
void machines_add_top_node (Machine *mach);
void machines_add_to_midi_channel (Machine *mach, gint channel);

void machine_init (Machine *);
void machine_free (Machine *);

void parent_machine_init (Machine *mach);
void parent_machine_set_child (Machine *mach, Machine *child);
void parent_machine_do_stuff (Machine *mach);

extern int G_sample_rate;
extern int G_buf_size;

static inline void
machine_clear_buffer (Machine *mach)
{
  memset (mach->out.data, 0, 
	  G_buf_size * sizeof (MSample));
}

#endif /* _MACHINES_H_ */
