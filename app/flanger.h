/* flanger.h - a digital flanger for machines
 *
 * Copyright (C) 1999 Henrik Nyman
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

#define FLANGER(mach)	        ((Flanger*)(mach))

typedef struct _Flanger		Flanger;
typedef enum _SweepWaveform	SweepWaveform;

enum _SweepWaveform
{
  SWEEP_SINE,
  SWEEP_TRIANGLE
};

struct _Flanger
{
  ParentMachine mach;		/* inheritance */

  float mix;                    /* Amount of effect signal added to original */
  float delay;                   /* Minimum delay */
  float sweep_depth;            /* Amplitude of sweep-wave added to delay 
				 * (Total delay = delay + sweep_depth) */

  float rate;                   /* Rate of sweep */
  SweepWaveform waveform;       /* Waveform of sweep */

  float feedback;               /* Amount of effect output routed back to
                                 * its input. */

  MSample *buf;			/* delay buffer */
  gint bufsize;			/* size of buf */
  gint i;			/* current pointer in buf */
  float angle;                  /* current sweep */
};


Machine *flanger_new (float mix, float delay, float sweep_depth, float rate,
		      SweepWaveform waveform, float feedback );




