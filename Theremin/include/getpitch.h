#ifndef GETPITCH_H
#define GETPITCH_H


#include <iostream>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <stdlib.h>
#include <unistd.h>
#include <iterator>
#include "portaudiocpp/PortAudioCpp.hxx" // [1] from PortAudio library
#include "AudioBuffer.h" // [2] from PortAudioRecPlay
#include <aubio.h> //[3] from Aubio

/* Above references [1],[2] to "PortAudioRecPlay" are found at http://keithv.com/software/portaudio/.
This example program also uses code from the PortAudio Library found at http://portaudio.com */

/* Above reference [3] is referring to "Aubio" (aubio 0.4.0) are from aubio.org/downloads */

//Some constants from PortAudioRecPlay.cpp
const double	SAMPLE_RATE			= 16000.0;
const int		FRAMES_PER_BUFFER	= 64;
//const int		TABLE_SIZE			= 200;

//typedef float fvec_t;
typedef unsigned int uint_t;
typedef int (*mmp_get_data) (fvec_t * input, uint_t hop_size);





extern "C"
{
void examples_common_init();
float examples_common_process(mmp_get_data   getData);
}

class GetPitch
{
public:
    GetPitch();
    float record(long millisec);
    /*This method converts the array of data taken from the PortAudioRecPlay program into data to be used by Aubio for pitch detection and recognition.
          The data it is passed is the data that is recorded using the main method of this program after one presses enter a second a time after making a short
        recording of the sound that you want to analyse*/

    AudioBuffer objAudioBuffer;

    VECTOR_SHORT_ITER it;
    VECTOR_SHORT vec;
    bool set;

private:

    // Create an object that is used to record, save to file and play the audio.


};





extern int convertArray(fvec_t * ibuf, uint_t hop_size);


#endif // GETPITCH_H
