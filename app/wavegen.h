/* wavegen.h - part of machines
 * A simple wave generator (or oscillator)
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

#define WAVEGEN(x)	((WaveGen*)(x))

typedef struct _WaveGen		WaveGen;
typedef enum _WaveGenType	WaveGenType;

enum _WaveGenType
{
  WAVEGEN_SQUARE,
  WAVEGEN_SAWTOOTH,
  WAVEGEN_SINE
};

struct _WaveGen
{
  Machine mach;			/* inheritance */

  GTree *notes;			/* running notes */
  WaveGenType type;
};

Machine *wavegen_new (WaveGenType);

