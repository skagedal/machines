/* trying not to overflow...
 * see resample.txt
 *
 * simon 1999-02-28
 */

void
main (void)
{
  unsigned int base_freq_of_sound = 44100;
  unsigned int note_freq = 20000;
  unsigned int play_rate = 44100;
  unsigned int base_note_freq = 30;
  unsigned int accuracy = 12;

  unsigned int speed;
  unsigned int i, index, length;

  speed = base_freq_of_sound * note_freq;
  printf ("%u * %u = %u\n", base_freq_of_sound, note_freq, speed);

  speed /= base_note_freq;
  printf ("/= %u = %u\n", base_note_freq, speed);

  speed <<= accuracy;
  printf ("<<= %u = %u\n", accuracy, speed);

  speed /= play_rate;
  printf ("/= %u = %u\n", play_rate, speed);

  /* just checking: */
  printf ("1 << %u = %u\n", accuracy, 1 << accuracy);

  printf ("interpolation:");

  length = 10000 << accuracy;
  for (i=0; i<length; i+= speed)
    {
      index = i >> accuracy;
      printf ("%u / %u\n", i % (1<<accuracy), (1<<accuracy));
    }
}

