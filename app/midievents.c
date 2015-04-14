/*
 * midimsg.c -- Midi event parsing code
 *
 * $Id: midievents.c,v 1.5 1998/02/06 09:49:31 johns Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "midi.h"
#include "midievents.h" 

MidiHandle *
midi_handle_new(void (*handler) (MidiHandle *))
{
  MidiHandle * handle;

  handle = g_new0 (MidiHandle, 1);

  handle->handler = handler;
 
  return handle;
} 

void
midi_handle_free (MidiHandle * handle)
{
  g_free (handle);
} 

void 
midi_add_byte (MidiHandle *handle, guchar b)
{
  if (b & MIDI_STATUS_MASK)
    {
    switch (b & MIDI_STATUS_TYPE_MASK)
      {
      case MIDI_NOTE_OFF:
      case MIDI_NOTE_ON:
      case MIDI_KEY_PRESSURE:
      case MIDI_CTRL_CHANGE:
      case MIDI_PITCH_BEND:
        handle->buf[0] = b;
        handle->bufpos=1;
        handle->buflen=3; 
        break;

      /* Messages with 1 data byte */
      case MIDI_PROG_CHANGE:
      case MIDI_CHAN_PRESSURE:
        handle->buf[0] = b;
        handle->bufpos=1;
        handle->buflen=2; 
        break;

      /* Special MIDI messages, can be varying length */
      case MIDI_SYSEX: /* should dispatch handlers for this stuff */
if (b != 0xf8)
  printf("System Message: %2x\n", b); 
        handle->buf[0]=0;
        handle->bufpos=0;
        handle->buflen=0;
        break;
        
      /* Out of sync, got data without matching status */
      default: 
printf("Unknown MIDI Status Message\n");
        handle->buf[0]=0;
        handle->bufpos=0;
        handle->buflen=0;
        break;
    }

    return;
  } 

  /* got a MIDI data byte, but didn't get the status byte that went with it */
  if (handle->buf[0] == 0) 
    return; 

  if (handle->bufpos < handle->buflen) {
    handle->buf[handle->bufpos] = b; /* add data byte to buffer */
    handle->bufpos++;
  }

  if (handle->bufpos >= handle->buflen) {
    handle->handler(handle);		/* do something with the MIDI event */
    handle->bufpos=1;
  }
}
