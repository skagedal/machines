/* midi.h - part of midikb
 * Copyright (C) 1997 Simon Kågedal
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

#ifndef _MIDI_H_
#define _MIDI_H_

/* status bytes
 */

#define MIDI_NOTE_OFF		0x80
#define MIDI_NOTE_ON		0x90
#define MIDI_KEY_PRESSURE	0xa0
#define MIDI_CTRL_CHANGE	0xb0
#define MIDI_PROG_CHANGE	0xc0
#define MIDI_CHAN_PRESSURE	0xd0
#define MIDI_PITCH_BEND		0xe0
#define MIDI_SYSEX	        0xf0

#define MIDI_STATUS_MASK	0x80
#define MIDI_STATUS_TYPE_MASK	0xf0
#define MIDI_CHAN_MASK		0x0f
#define MIDI_DATA_MASK		0x7f

/* control change
 */

#define	MIDIC_LOCAL_CONTROL	122
#define	MIDIC_ALL_NOTES_OFF	123
#define MIDIC_OMNI_MODE_OFF	124		/* these four also	*/
#define MIDIC_OMNI_MODE_ON	125		/* cause		*/
#define MIDIC_MONO_MODE		126		/* all notes off	*/
#define MIDIC_POLY_MODE		127		/*			*/

/*
 * 0x80-0x8f     note off       2 - 1 byte pitch, followed by 1 byte velocity
 * 0x90-0x9f     note on        2 - 1 byte pitch, followed by 1 byte velocity
 * 0xa0-0xaf     key pressure   2 - 1 byte pitch, 1 byte pressure (after-touch)
 * 0xb0-0xbf     parameter      2 - 1 byte parameter number, 1 byte setting
 * 0xc0-0xcf     program        1 byte program selected
 * 0xd0-0xdf     chan. pressure 1 byte channel pressure (after-touch)
 * 0xe0-0xef     pitch wheel    2 bytes giving a 14 bit value, least
 *                                    significant 7 bits first
 */

#endif /* _MIDI_H_ */
