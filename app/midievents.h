/*
 * midievents.h -- Midi event parsing code
 *
 * $Id: midievents.h,v 1.3 1998/02/06 09:28:33 johns Exp $
 *
 */

#ifndef _MIDIEVENTS_H_
#define _MIDIEVENTS_H_

#define MIDI_GET_STATUS(x)	((x)->buf[0] & MIDI_STATUS_TYPE_MASK)
#define MIDI_GET_CHANNEL(x)	((x)->buf[0] & MIDI_CHAN_MASK)
#define MIDI_GET_NOTE(x)	((x)->buf[1] & MIDI_DATA_MASK)
#define MIDI_GET_VELOCITY(x)	((x)->buf[2] & MIDI_DATA_MASK)
#define MIDI_GET_CONTROLLER(x)	((x)->buf[1] & MIDI_DATA_MASK)
#define MIDI_GET_VALUE(x)	((x)->buf[2] & MIDI_DATA_MASK)
#define MIDI_GET_PROGRAM(x)	((x)->buf[1] & MIDI_DATA_MASK)
#define MIDI_GET_CHAN_PRESSURE(x)	((x)->buf[1] & MIDI_DATA_MASK)
#define MIDI_GET_PITCH_BEND(x)	(((x)->buf[1] & MIDI_DATA_MASK) | \
				 (((x)->buf[2] & MIDI_DATA_MASK) << 7))


struct _MidiHandle {
  unsigned char buf[4]; /* Buffer for MIDI messages */
  int bufpos;
  int buflen;
  void (*handler) (struct _MidiHandle *handle); /* callback */
  void *extradata; /* that the user might want to put in */
};

typedef struct _MidiHandle MidiHandle;

MidiHandle *midi_handle_new (void (*handler) (MidiHandle *));
void midi_handle_free (MidiHandle *handle);
void midi_add_byte (MidiHandle *handle, unsigned char b);
void midi_default_handler (MidiHandle *handle);

#if 0
int midi_get_status (MidiHandle *handle);
int midi_get_channel (MidiHandle *handle);
int midi_get_note (MidiHandle *handle);
int midi_get_velocity (MidiHandle *handle);
int midi_get_controller (MidiHandle *handle);
int midi_get_value (MidiHandle *handle);
int midi_get_program (MidiHandle *handle);
int midi_get_chan_pressure (MidiHandle *handle);
int midi_get_pitch_bend (MidiHandle *handle);
#endif

#endif
