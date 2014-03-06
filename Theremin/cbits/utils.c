/*
  Copyright (C) 2003-2013 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

/**

  This file includes some tools common to all examples. Code specific to the
  algorithm performed by each program should go in the source file of that
  program instead.

*/

#include "aubioutils.h"
#ifdef HAVE_JACK
//#include "jackio.h"
#endif /* HAVE_JACK */


#define  pitchArraySize 200
float pitches[pitchArraySize];
int pitchCount;
int start=0;
int end=0;

float pitchTotal=0;
float pitchAverage=0;
int verbose = 0;
int usejack = 0;
// input / output
char_t *sink_uri = NULL;
char_t *source_uri = NULL;
// general stuff
uint_t samplerate = 16000;
uint_t buffer_size = 512;
uint_t hop_size = 256;
// onset stuff
char_t * onset_method = "default";
smpl_t onset_threshold = 0.0; // will be set if != 0.
// pitch stuff
char_t * pitch_unit = "default";
char_t * pitch_method = "default";
smpl_t pitch_tolerance = 0.0; // will be set if != 0.
// tempo stuff
char_t * tempo_method = "default";
// more general stuff
smpl_t silence_threshold = -90.;
uint_t mix_input = 0;

uint_t force_overwrite = 0;

//
// internal memory stuff
aubio_source_t *this_source = NULL;
aubio_sink_t *this_sink = NULL;
fvec_t *ibuf;
fvec_t *obuf;

/* settings */
int blocks = 0;

extern void usage (FILE * stream, int exit_code);
extern int parse_args (int argc, char **argv);

#if HAVE_JACK
aubio_jack_t *jack_setup;
#endif

void
printbuff()
{
    {
    int i=0;
    for(i=0; i<(hop_size); i++)

        printf("%f ", fvec_get_sample(ibuf, i));
    }
    printf ("\n");
}

//aubiopitch set up (extra vectors needed for aubio as well as the wavetable) - also vector for outlier removal
aubio_pitch_t *o;
aubio_wavetable_t *wavetable;
fvec_t *pitch;
vector v;


//command line argument parsing and initialisation of vectors ibuf and obuf, as well as the wavetable used by aubio used in pitch detection
void examples_common_init ()
{
    vector_init(&v);
    buffer_size = 2048;

    ibuf = new_fvec (hop_size);
    obuf = new_fvec (hop_size);

    wavetable = new_aubio_wavetable (samplerate, hop_size);
    aubio_wavetable_play ( wavetable );

    o = new_aubio_pitch (pitch_method, buffer_size, hop_size, samplerate);
    if (pitch_tolerance != 0.)
      aubio_pitch_set_tolerance (o, pitch_tolerance);
    if (silence_threshold != -90.)
      aubio_pitch_set_silence (o, silence_threshold);
    if (pitch_unit != NULL)
      aubio_pitch_set_unit (o, pitch_unit);

    pitch = new_fvec (1);



}

void examples_common_del (void)
{
  del_fvec (ibuf);
  del_fvec (obuf);
  aubio_cleanup ();
  fflush(stderr);
  fflush(stdout);
}



//aubiopitch methods (used by Aubio to garner pitch in Hz from data)

//This method processes one "block" of audio data passed from the buffer to acquire a pitch for this specific block
void process_block(fvec_t * ibuf, fvec_t * obuf) {
  fvec_zeros(obuf);
  aubio_pitch_do (o, ibuf, pitch);
  smpl_t freq = fvec_get_sample(pitch, 0);
  aubio_wavetable_set_amp ( wavetable, aubio_level_lin (ibuf) );
  aubio_wavetable_set_freq ( wavetable, freq );

  if (mix_input)
    aubio_wavetable_do (wavetable, ibuf, obuf);
  else
    aubio_wavetable_do (wavetable, obuf, obuf);
}

//Prints the pitch, taken from the vector containing the pitch data
void process_print (void)
{
  smpl_t pitch_found = fvec_get_sample(pitch, 0);
  outmsg("%f %f\n",(blocks)
         *hop_size/(float)samplerate, pitch_found);
}


//Calculates an average, (maybe deprecated when outlier removal method is completed)
void standardisePitch()
{
  int i=0;
  int range=end-start;
  for(i=start; i<end; i++)
  {
       float current=pitches[i];
      //printf("Current vector value: %f at index %i", *current, i);
      pitchTotal+=current;


  }
 pitchAverage=pitchTotal/range;
 printf("Pitch total is: %f \n", pitchTotal);


// for(i=0; i<pitchCount; i++)
// {
//     printf("base array %i: %f\n", i, pitches[i]); debugging code
// }

}

//Adds to the vector and sorts data using insertion sort to allow for proper averaging/outlier removal
//TODO - find a good place to call this method, finish outlier removal method
void populateAndSortArray()
{

    if(pitchCount<pitchArraySize)
    {
        pitches[pitchCount] = fvec_get_sample(pitch, 0);
        pitchCount++;
    }
    else{
        printf("Out of space in pitches array, need to expand it");
    }

    //check and sort from lowest to highest using insertion sort
    int i, j;
    float datum;
    for (i = 1; i < pitchCount; ++i)
    {
        datum = pitches[i];
         for (j = i; j > 0; j--)
         {
             float nextValue=pitches[j-1];
             if(nextValue > datum)
                 pitches[j]=nextValue;
             else break;
         }

         pitches[j]=datum;
    }

}

//Method calculates the median, interquartile range, and also the inner and outer fences to determine whether data is an outlier or not.
//Then it ignores outliers in the array
void ignoreOutliers()
{
    int i=0;
    int middle=pitchCount/2;  //index in vector
   // float centre = vector_get(&v, middle);
   // float offcentre = vector_get(&v, middle-1);
   //float median=vector_count(&v) % 2 == 0 ? (centre + offcentre)/2 : centre;

    int Q1index = pitchCount / 4;
    int Q3index = Q1index + middle;

    float Q1ref = pitches[Q1index];
    float Q1offref = pitches[Q1index-1];
    float Q3ref = pitches[Q3index];
    float Q3offref = pitches[Q3index-1];

    float Q1=pitchCount % 2 == 0 ? (Q1ref + Q1offref )/2 : Q1ref;
    float Q3=pitchCount % 2 == 0 ? (Q3ref + Q3offref)/2 : Q3ref;

   // float iqRangeInner=(Q3-Q1)*1.5;
   // float innerFence1= Q1-iqRangeInner; //For mild outliers
   // float innerFence2= Q3+iqRangeInner; //For mild outliers

    float iqRangeOuter=(Q3-Q1)*3;
    float outerFence1= Q1-iqRangeOuter; //For extreme outliers - likely to be used more than the innerfences
    float outerFence2= Q3+iqRangeOuter; // ** as above


    while(pitches[i]<outerFence1)
        i++;
    while(pitches[i]==0)
    {
        i++;
    }
    start = i;
    while(pitches[i]<outerFence2)
        i++;
     end = i-1;
}


//Method used primarily for processing the data to get a pitch in Hz - currently returns an average pitch
float examples_common_process ( mmp_get_data   getData)
{

  uint_t read = hop_size;

    /* phasevoc */
    blocks = 0;
    uint_t total_read = 0;

    int hasData;
    do {
        hasData = getData(ibuf, hop_size);
        if(hasData==0)
        {
            read = 0;
        }
      //aubio_source_do (this_source, ibuf, &read);
      //printf("Pre process: ");                      //CLEANUP REQUIRED FOR PRINT STATEMENTS CTRL-F printbuff()
      //printbuff();
      process_block (ibuf, obuf);

        populateAndSortArray();


      //printf("Post process: ");                     //these print lines have been commented out but not removed due to testing purposes
      //printbuff();
      // print to console if verbose or no output given
      if (verbose || sink_uri == NULL) {
        process_print();
      }
      if (this_sink) {
        aubio_sink_do (this_sink, obuf, hop_size);
      }
      blocks++;
      total_read += read;
    } while (read == hop_size);

    ignoreOutliers();
    standardisePitch();

    verbmsg ("read %.2fs (%d samples in %d blocks of %d) from %s at %dHz\n",
        total_read * 1. / samplerate,
        total_read, blocks, hop_size, source_uri, samplerate);

    del_aubio_source (this_source);
    del_aubio_sink   (this_sink);
    pitchTotal=0;
    return pitchAverage;

  }



void
send_noteon (int pitch, int velo)
{
  smpl_t mpitch = floor (aubio_freqtomidi (pitch) + .5);
#if HAVE_JACK
  jack_midi_event_t ev;
  ev.size = 3;
  ev.buffer = malloc (3 * sizeof (jack_midi_data_t)); // FIXME
  ev.time = 0;
  if (usejack) {
    ev.buffer[2] = velo;
    ev.buffer[1] = mpitch;
    if (velo == 0) {
      ev.buffer[0] = 0x80;      /* note off */
    } else {
      ev.buffer[0] = 0x90;      /* note on */
    }
    aubio_jack_midi_event_write (jack_setup, (jack_midi_event_t *) & ev);
  } else
#endif
  if (velo == 0) {
    verbmsg ("%f\n", blocks * hop_size / (float) samplerate);
  } else {
    verbmsg ("%f\t%f\t", mpitch, blocks * hop_size / (float) samplerate);
  }
}

