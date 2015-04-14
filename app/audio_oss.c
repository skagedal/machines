/* audio_oss.c - part of machines
 * OSS audio handling functions
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

/* for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>		/* for close */
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <glib.h>

#include "machines.h"
#include "audio_oss.h"

int audio_fd;

gint
audio_oss_init (gchar *devname)
{
  int format, stereo;
  int numfrags = 2;
  int fragsize = 11;
  int fragmentsize = (numfrags << 16) | fragsize;
  int sample_rate;

  /*  audio_fd = open (devname, O_WRONLY | O_NDELAY, 0); */
  audio_fd = open (devname, O_WRONLY, 0);

  if (audio_fd == -1)
    {
      fprintf (stderr, "%s: %s: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  /* Fragment size */

  if (ioctl (audio_fd, SNDCTL_DSP_SETFRAGMENT, &fragmentsize) == -1)
    {
      fprintf (stderr, "%s: %s: SNDCTL_DSP_SETFRAGMENT: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  /* Audio format */

  format = AFMT_S16_LE;		/* use AFMT_S16_NE ? */

  if (ioctl (audio_fd, SNDCTL_DSP_SETFMT, &format) == -1)
    {
      fprintf (stderr, "%s: %s: SNDCTL_DSP_SETFMT: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  if (format != AFMT_S16_LE)
    {
      fprintf (stderr, "%s: %s: doesn't support 16 bit little-endian format",
	       g_get_prgname (), devname);
    }

  /* Number of channels */

  stereo = 0;

  if (ioctl (audio_fd, SNDCTL_DSP_STEREO, &stereo) == -1)
    {
      fprintf (stderr, "%s: %s: SNDCTL_DSP_STEREO: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  if (stereo != 0)
    {
      fprintf (stderr, "%s: %s: can't set to mono!",
	       g_get_prgname (), devname);
    }

  /* Sampling rate (speed) */

  sample_rate = G_sample_rate;

  if (ioctl (audio_fd, SNDCTL_DSP_SPEED, &sample_rate) == -1)
    {
      fprintf (stderr, "%s: %s: SNDCTL_DSP_SPEED: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  if (sample_rate != G_sample_rate)
    {
      fprintf (stderr, "%s: %s: sampling rate: %i\n", 
	       g_get_prgname (), devname, sample_rate);
      /* maybe machines_set_sample_rate (sample_rate) ? */
    }

  if (ioctl (audio_fd, SNDCTL_DSP_GETBLKSIZE, &fragmentsize) == -1)
    {
      fprintf (stderr, "%s: %s: SNDCTL_DSP_GETBLKSIZE: %s\n",
	       g_get_prgname (), devname, g_strerror (errno));
      return 1;
    }

  g_print ("fragment size: %i\n", fragmentsize);

  return 0;
}

void
audio_oss_close (void)
{
  close (audio_fd);
}

#if 0
void
audio_oss_loop (void)
{
  /* THIS IS JUST TO TEST */

  int samples_per_wave;
  gint16 *buffer;
  int samples, i;

  int sample_rate = 44100;	/* samples per second */
  int tone = 440;		/* tone to play (base A) */

  samples_per_wave = sample_rate / tone;

  samples = samples_per_wave;
  buffer = g_new (gint16, samples);

  /* generate square wave */

  for (i = 0; i < (samples/2); ++i)
    {
      buffer[i] = 10000;
      buffer[samples-i-1] = -10000;
      /*
      buffer[i] = 20000;
      buffer[samples-i-1] = 0;
      */
    }

  /* play */

  while (1)
    {
      write (audio_fd, buffer, sizeof(gint16) * samples);
    }
}
#endif
