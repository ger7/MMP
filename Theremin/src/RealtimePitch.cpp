#include <iostream>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <stdlib.h>
#include <iterator>
#include "portaudiocpp/PortAudioCpp.hxx" // [1] from PortAudio library
#include "AudioBuffer.h" // [2] from PortAudioRecPlay
//#include "aubioutils.h"  // [4] from Aubio library
//#include "parse_args.h" // [3] from Aubio library

#include <aubio.h>


/* Above references [1][3][4] are referring to "Aubio" (aubio 0.4.0) are from aubio.org/downloads */
/* Above references [2] to "PortAudioRecPlay" are found at http://keithv.com/software/portaudio/. This example program also uses code from the PortAudio Library found at http://portaudio.com */

//Some constants from PortAudioRecPlay.cpp
const double	SAMPLE_RATE			= 16000.0;
const int		FRAMES_PER_BUFFER	= 64;
const int		TABLE_SIZE			= 200;


//typedef float fvec_t;
typedef unsigned int uint_t;
typedef int (*mmp_get_data) (fvec_t * input, uint_t hop_size);

// Create a object that is used to record, save to file and play the audio.
AudioBuffer	objAudioBuffer((int) (SAMPLE_RATE * 60));

extern "C" {
void examples_common_init();
void examples_common_process(mmp_get_data   getData);
//void fvec_set_sample(fvec_t*, fvec_t, uint_t);

}

VECTOR_SHORT_ITER it;
VECTOR_SHORT vec;
bool set =false;
int convertArray(fvec_t * ibuf, uint_t hop_size)
{
    int hasData = 0;
    if(!set)
    {
        it = objAudioBuffer.getIterator();
        vec = objAudioBuffer.getVectors();
        set = true;
    }

    int count =0;

    for(uint_t i=0;i<hop_size;i++)
    {
        float pitchdata = 0.0;
        if(it!=vec.end())
        {
            pitchdata=(float)(*it)/10000.00;
            it++;
            hasData=1;
        }
        else
        {
            hasData=0;
        }

        if(pitchdata==0)
        {
            count ++;
        }
        else
        {
            count=0;
        }
        if(count>=5)
        {
            hasData=0;
        }
        fvec_set_sample(ibuf, pitchdata, i);


    }
    return hasData;
}




int main(int argc, char* argv[])
{
    char 	chWait;
    int 	iInputDevice = -1;




    //set up portaudio
    portaudio::AutoSystem autoSys;
    portaudio::System &sys = portaudio::System::instance();

    //set up default input and output devices
    if (argc > 2)
    {
        iInputDevice 	= atoi(argv[1]);

        cout << "Using input device index = " << iInputDevice << endl;
    }
    else
    {
        cout << "Using system default input/output devices..." << endl;
        iInputDevice	= sys.defaultInputDevice().index();
    }


    // List out all the devices we have
    int 	iNumDevices 		= sys.deviceCount();
    int 	iIndex 				= 0;
    string	strDetails			= "";

    std::cout << "Number of devices = " << iNumDevices << std::endl;
    if ((iInputDevice >= 0) && (iInputDevice >= iNumDevices))
    {
        cout << "Input device index out of range!" << endl;
        return 0;
    }


    //		portaudio::Device inDevice = portaudio::Device(sys.defaultInputDevice());

    //		portaudio::Device& inDevice 	= sys.deviceByIndex(iInputDevice);

    for (portaudio::System::DeviceIterator i = sys.devicesBegin(); i != sys.devicesEnd(); ++i)
    {
        strDetails = "";
        if ((*i).isSystemDefaultInputDevice())
            strDetails += ", default input";

        cout << (*i).index() << ": " << (*i).name() << ", ";
        cout << "in=" << (*i).maxInputChannels() << " ";
        cout << "out=" << (*i).maxOutputChannels() << ", ";
        cout << (*i).hostApi().name();

        cout << strDetails.c_str() << endl;

        iIndex++;
    }

    cout << "Press enter to START recording";
    cin.get(chWait);

    //open recording input stream
    cout << "Opening recording input stream on " << sys.deviceByIndex(iInputDevice).name() << endl;
    portaudio::DirectionSpecificStreamParameters inParamsRecord(sys.deviceByIndex(iInputDevice), 1, portaudio::INT16, false, sys.deviceByIndex(iInputDevice).defaultLowInputLatency(), NULL);
    portaudio::StreamParameters paramsRecord(inParamsRecord, portaudio::DirectionSpecificStreamParameters::null(), SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);
    portaudio::MemFunCallbackStream<AudioBuffer> streamRecord(paramsRecord, objAudioBuffer, &AudioBuffer::RecordCallback);

    cout << "Press enter to STOP recording.";
    streamRecord.start();
    cin.get(chWait);
    streamRecord.stop();

    objAudioBuffer.ResetPlayback();


    //AUBIO PITCH PROCESSING
    examples_common_init();
    examples_common_process((mmp_get_data)convertArray);
}
