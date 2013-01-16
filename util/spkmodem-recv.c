#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compilation:  gcc -o spkmodem-recv spkmodem-recv  */
/* Usage: parecord --channels=1 --rate=48000 --format=s16le | ./spkmodem-recv */

#define RATE 48000
#define SAMPLES_PER_TRAME 480
#define AMPLITUDE_THRESHOLD 100000
#define FREQ_SEP_MIN 15
#define FREQ_SEP_NOM 20
#define FREQ_SEP_MAX 25

#define FREQ_DATA_MIN 10
#define FREQ_DATA_THRESHOLD 60
#define FREQ_DATA_MAX 120
#define AMPLITUDE_SAMPLES 2 * SAMPLES_PER_TRAME

#define THRESHOLD 1000

#define DEBUG 0

static signed short trame[2 * SAMPLES_PER_TRAME];
static signed short pulse[2 * SAMPLES_PER_TRAME];
static int ringpos = 0;
static int pos, f1, f2;
static int amplitude = 0;
static int lp = 0;

static void
read_sample (void)
{
  amplitude -= abs (trame[ringpos]);
  f1 -= pulse[ringpos];
  f1 += pulse[(ringpos + SAMPLES_PER_TRAME) % (2 * SAMPLES_PER_TRAME)];
  f2 -= pulse[(ringpos + SAMPLES_PER_TRAME) % (2 * SAMPLES_PER_TRAME)];
  fread (trame + ringpos, 1, sizeof (trame[0]), stdin);
  amplitude += abs (trame[ringpos]);

  if (pos ? (trame[ringpos] < -THRESHOLD)
      : (trame[ringpos] > +THRESHOLD))
    {
      pulse[ringpos] = 1;
      pos = !pos;
      f2++;
    }
  else
    pulse[ringpos] = 0;
  ringpos++;
  ringpos %= 2 * SAMPLES_PER_TRAME;
  lp++;
}

int
main ()
{
  int bitn = 7;
  char c = 0;
  int i;
  while (!feof (stdin))
    {
      if (lp > 20000)
	{
	  fflush (stdout);
	  bitn = 7;
	  c = 0;
	  lp = 0;
	}
      if (f2 > 12 && f2 < 25
	  && f1 > 5 && f1 < 120)
	{
#if DEBUG
	  printf ("%d %d %d @%d\n", f1, f2, FREQ_DATA_THRESHOLD,
		  ftell (stdin) - sizeof (trame));
#endif
	  if (f1 < 60)
	    c |= (1 << bitn);
	  bitn--;
	  if (bitn < 0)
	    {
#if DEBUG
	      printf ("<%c, %x>", c, c);
#else
	      printf ("%c", c);
#endif
	      bitn = 7;
	      c = 0;
	    }
	  lp = 0;
	  for (i = 0; i < SAMPLES_PER_TRAME; i++)
	    read_sample ();
	  continue;
	}
      read_sample ();
    }
  return 0;
}
