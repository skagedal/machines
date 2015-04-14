/* utility functions
 */

#include <config.h>
#include <math.h>
#include "util.h"

/* `note_frequency' calculates the base frequency of a given MIDI note
 * number... maybe...
 */

int
note_frequency (int note)
{
  double n = note - 57;	/* 57 == a4 */

  return ((int)pow(2.0, log(440.0)/log(2.0) + (n / 12.0)));
}
