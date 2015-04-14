/* main.c - part of machines
 *
 * Copyright (C) 1998,1999 Simon Kågedal
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
#include <stdlib.h>
#include <glib.h>
#include <getopt.h>
#include "machines.h"
#include "audio_oss.h"

/* print usage information and exit
 */

static void
usage_exit (int ret_code)
{
  printf ("Usage: %s [OPTION]...\n\n"
	  "  -c FILENAME, --config=FILENAME\tread config from FILENAME\n"
	  "  -r RATE, --playrate=RATE\t\tset sample rate to RATE\n"
	  "  -b BUFSIZE, --bufsize=BUFSIZE\t\tset buffer size to BUFSIZE\n"
	  "  -h, --help\t\t\t\tdisplay this help and exit\n"
	  "  -v, --version\t\t\t\tdisplay version information and exit\n",
	  g_get_prgname ());

  exit (ret_code);
}

int
main (int argc, char *argv[])
{
  int c, option_index = 0;

  GString *config_filename = NULL;
  
  struct option opts[] =
  {
    { "config", required_argument, NULL, 'c' },
    { "playrate", required_argument, NULL, 'r' },
    { "bufsize", required_argument, NULL, 'b' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { 0, 0, 0, 0 }
  };

  g_set_prgname (argv [0]);

  /* parse options */

  while (1)
    {
      c = getopt_long (argc, argv, "c:r:b:hv", opts,
		       &option_index);

      if (c == -1)
	break;

      switch (c)
	{
	case 'c':
	  config_filename = g_string_new (optarg);
	  break;
	  
	case 'r':
	  G_sample_rate = atoi (optarg);
	  break;

	case 'b':
	  G_buf_size = atoi (optarg);
	  break;

	case 'h':
	  usage_exit (0);

	case 'v':
	  fputs ("machines " VERSION "\n", stdout);
	  exit (0);

	case '?':
	  /* `getopt_long' already printed an error message. */
	  fprintf (stderr, "Try `%s --help' for more information.\n",
		   g_get_prgname ());
	  exit (1);

	default:
	  abort ();
	}
    }

  if (!config_filename)
    {
      config_filename = g_string_new (g_get_home_dir ());
      config_filename = g_string_append (config_filename, "/.machines");
    }
  
  if (!machines_init (config_filename->str))
    return 0;

  g_string_free (config_filename, TRUE);

  if (audio_oss_init ("/dev/dsp"))
    exit (1);

  machines_loop ();

  machines_free ();

  audio_oss_close ();

  return 0;
}
