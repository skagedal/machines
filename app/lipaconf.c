/* lipaconf.c - part of machines
 *
 * Configure machines with LIPA
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

#include <stdio.h>
#include <glib.h>
#include <lipa/lipa.h>

#include "machines.h"
#include "speaker.h"
#include "disk.h"
#include "mixer.h"
#include "wavegen.h"
#include "wavetable.h"
#include "delay.h"
#include "flanger.h"

static LObject *
lisp_wavegen_new (LObject *args)
{
  LObject *symname;
  LObject *obj;
  WaveGenType type = WAVEGEN_SINE; /* or whatever */
  Machine *mach;

  gchar *msg = "wavegen-new wants one argument:\n"
    "'wavegen-sine, 'wavegen-square, 'wavegen-sawtooth";

  if (!lipa_list_length (args, 1) ||
      !SYMBOLNAMEP (lipa_car (args)))
    {
      g_message (msg);
      return NULL;
    }

  symname = lipa_car (args);

  if (!strcmp ("wavegen-sine", L_SYMBOLNAME (symname)->str))
    {
      type = WAVEGEN_SINE;
    }
  else if (!strcmp ("wavegen-square", L_SYMBOLNAME (symname)->str))
    {
      type = WAVEGEN_SQUARE;
    }
  else if (!strcmp ("wavegen-sawtooth", L_SYMBOLNAME (symname)->str))
    {
      type = WAVEGEN_SAWTOOTH;
    }
  else
    {
      g_message (msg);
      return NULL;
    }

  mach = wavegen_new (type);

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_wavetable_new_from_file (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 2) ||
      !STRINGP (lipa_car (args)) ||
      !INTP (lipa_cadr (args)))
    {
      g_message ("wavetable-new-from-file wants two arguments:\n"
		 "FILENAME BASE-NOTE\n");
      return NULL;
    }
  
  mach = wavetable_new_from_file (L_STRING (lipa_car (args))->str,
				  L_INT (lipa_cadr (args)));

  if (!mach)
    {
      g_message ("failed in creating wavetable\n");
      return NULL;
    }

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_wavetable_new_from_file_looped (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 4) ||
      !STRINGP (lipa_car (args)) ||
      !INTP (lipa_cadr (args)) ||
      !INTP (lipa_caddr (args)) ||
      !INTP (lipa_caddr (lipa_cdr (args))))
    {
      g_message ("wavetable-new-from-file-looped wants four arguments:\n"
		 "FILENAME BASE-NOTE LOOP-START LOOP-END\n");;
      return NULL;
    }

  mach = wavetable_new_from_file_looped (L_STRING (lipa_car (args))->str,
					 L_INT (lipa_cadr (args)),
					 L_INT (lipa_caddr (args)),
					 L_INT (lipa_caddr (lipa_cdr (args))));

  if (!mach)
    {
      g_message ("failed in creating looped wavetable\n");
      return NULL;
    }

  machines_register (mach);
  
  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_mixer_new (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 0))
    {
      g_message ("mixer-new doesn't want any arguments\n");
      return NULL;
    }

  mach = mixer_new ();

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_speaker_new (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 0))
    {
      g_message ("speaker-new doesn't want any arguments\n");
      return NULL;
    }

  mach = speaker_new ();

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_disk_new (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 1) ||
      !STRINGP (lipa_car (args)))
    {
      g_message ("disk-new wants one argument: FILENAME\n");
      return NULL;
    }

  mach = disk_new (L_STRING (lipa_car (args))->str);

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_delay_new (LObject *args)
{
  LObject *obj;
  Machine *mach;

  if (!lipa_list_length (args, 2) ||
      !FLOATP (lipa_car (args)) ||
      !INTP (lipa_cadr (args)))
    {
      g_message ("delay-new wants two arguments:\n"
		 "LENGTH DIVISOR");
      return NULL;
    }

  mach = delay_new (L_FLOAT (lipa_car (args)), 
		    L_INT (lipa_cadr (args)));

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_flanger_new (LObject *args)
{
  LObject *obj;
  Machine *mach;
  float mix, delay, sweepdepth, rate, feedback;
  SweepWaveform sweep = SWEEP_TRIANGLE;
  gchar *symname;

  gchar *msg = "flanger-new wants six arguments:\n"
    "MIXDEPTH DELAY-LENGTH SWEEP-DEPTH-LENGTH RATE WAVEFORM FEEDBACK\n"
    "where WAVEFORM is 'sweep-sine or 'sweep-triangle\n";
  
  if (!lipa_list_length (args, 6) ||
      !NUMBERP (lipa_car (args)) ||
      !NUMBERP (lipa_cadr (args)) ||
      !NUMBERP (lipa_caddr (args)) ||
      !NUMBERP (lipa_caddr (lipa_cdr (args))) ||
      !SYMBOLNAMEP (lipa_caddr (lipa_cdr (lipa_cdr (args)))) ||
      !NUMBERP (lipa_caddr (lipa_cdr (lipa_cdr (lipa_cdr (args))))))
    {
      g_message (msg);
      return NULL;
    }

  mix = L_NUMBER (lipa_car (args));
  delay = L_NUMBER (lipa_cadr (args));
  sweepdepth = L_NUMBER (lipa_caddr (args));
  rate = L_NUMBER (lipa_caddr (lipa_cdr (args)));
  symname = L_SYMBOLNAME (lipa_caddr (lipa_cdr (lipa_cdr (args))))->str;
  feedback = L_NUMBER (lipa_caddr (lipa_cdr (lipa_cdr (lipa_cdr (args)))));
  
  if (!strcmp ("sweep-sine", symname))
    {
      sweep = SWEEP_SINE;
    }
  else if (!strcmp ("sweep-triangle", symname))
    {
      sweep = SWEEP_TRIANGLE;
    }
  else
    {
      g_message (msg);
      return NULL;
    }

  /*  printf ("creating this flanger:\n"
	  "mix:\t%f\ndelay:\t%f\nsweepdepth:\t%f\nrate:\t%f\n"
	  "sweep:\t%s\nfeedback:\t%f\n",
	  mix, delay, sweepdepth, rate,
	  sweep == SWEEP_SINE ? "sine" : "triangle", feedback);
	  */
  mach = flanger_new (mix, delay, sweepdepth, rate, sweep, feedback);

  machines_register (mach);

  obj = lipa_new_userobject (mach);

  return obj;
}

static LObject *
lisp_set_child (LObject *args)
{
  Machine *parent;
  Machine *child;

  if (!lipa_list_length (args, 2) ||
      !USEROBJECTP (lipa_car (args)) ||
      !USEROBJECTP (lipa_cadr (args)))
    {
      g_message ("set-child wants two arguments:\n"
		 "PARENT CHILD\n");
      return NULL;
    }

  parent = MACHINE (L_USEROBJECT (lipa_car (args)));
  child = MACHINE (L_USEROBJECT (lipa_cadr (args)));

  parent_machine_set_child (parent, child);

  return NULL;
}

static LObject *
lisp_mixer_add_child (LObject *args)
{
  Machine *mixer;
  Machine *child;
  double mix;

  if (!lipa_list_length (args, 3) ||
      !USEROBJECTP (lipa_car (args)) ||
      !USEROBJECTP (lipa_cadr (args)) ||
      !FLOATP (lipa_caddr (args)))
    {
      g_message ("mixer-add-child wants three arguments:\n"
		 "MIXER CHILD MIX\n");
      return NULL;
    }

  mixer = MACHINE (L_USEROBJECT (lipa_car (args)));
  child = MACHINE (L_USEROBJECT (lipa_cadr (args)));
  mix = L_FLOAT (lipa_caddr (args));

  mixer_add_child (mixer, child, mix);

  return NULL;
}

static LObject *
lisp_add_top_node (LObject *args)
{
  Machine *mach;

  if (!lipa_list_length (args, 1) ||
      !USEROBJECTP (lipa_car (args)))
    {
      g_message ("add-top-node wants one argument:\n"
		 "MACHINE\n");
      return NULL;
    }

  mach = MACHINE (L_USEROBJECT (lipa_car (args)));

  machines_add_top_node (mach);

  return NULL;
}

static LObject *
lisp_add_to_midi_channel (LObject *args)
{
  Machine *mach;

  if (!lipa_list_length (args, 2) ||
      !USEROBJECTP (lipa_car (args)) ||
      !INTP (lipa_cadr (args)))
    {
      g_message ("add-to-midi-channel wants two arguments:\n"
		 "MACHINE CHANNEL\n");
      return NULL;
    }

  mach = MACHINE (L_USEROBJECT (lipa_car (args)));

  machines_add_to_midi_channel (mach,
				L_INT (lipa_cadr (args)));

  return NULL;
}

void
lipaconf_read_file (gchar *filename)
{
  lipa_init ();

  lipa_register_subr ("wavegen-new", lisp_wavegen_new);
  lipa_register_subr ("wavetable-new-from-file", lisp_wavetable_new_from_file);
  lipa_register_subr ("wavetable-new-from-file-looped",
		      lisp_wavetable_new_from_file_looped);
  lipa_register_subr ("mixer-new", lisp_mixer_new);
  lipa_register_subr ("speaker-new", lisp_speaker_new);
  lipa_register_subr ("disk-new", lisp_disk_new);
  lipa_register_subr ("delay-new", lisp_delay_new);
  lipa_register_subr ("flanger-new", lisp_flanger_new);
  lipa_register_subr ("set-child", lisp_set_child);
  lipa_register_subr ("mixer-add-child", lisp_mixer_add_child);
  lipa_register_subr ("add-top-node", lisp_add_top_node);
  lipa_register_subr ("add-to-midi-channel", lisp_add_to_midi_channel);

  lipa_load (filename);

  /* lipa_exit (); */
}
