#include <iostream>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <stdlib.h>
#include <iterator>
#include "portaudiocpp/PortAudioCpp.hxx" // [1] from PortAudio library
#include "AudioBuffer.h" // [2] from PortAudioRecPlay
#include "parse_args.h" // [3] from Aubio library
#include "utils.h"  // [4] from Aubio library
#include "aubio.h" // [5] from Aubio Library

/* Above references [3],[4] are referring to "Aubio" (aubio 0.4.0) are from aubio.org/downloads */
/* Above references [1],[2] to [1]"PortAudio Library" and [2]"PortAudioRecPlay". These are found at [1] www.portaudio.com and [2] http://keithv.com/software/portaudio/.*/




// Create a object that is used to record, save to file and play the audio.
AudioBuffer	objAudioBuffer((int) (SAMPLE_RATE * 60));




void convertArray(fvec_t * ibuf, uint_t hop_size)
{
    VECTOR_SHORT_ITER it = objAudioBuffer.getIterator();
    VECTOR_SHORT vec = objAudioBuffer.getVectors();
    for(uint_t i=0;i<hop_size;i++)
    {
        float pitchdata = 0.0;
        if(it!=vec.end())
        {
            pitchdata=(float)(*it)/10000.00;
            it++;
        }

        fvec_set_sample(ibuf, pitchdata, i);

    }
}

//aubiopitch set up
aubio_pitch_t *o;
aubio_wavetable_t *wavetable;
fvec_t *pitch;

//aubiopitch methods
void
process_block(fvec_t * ibuf, fvec_t * obuf) {
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

void
process_print (void) {
  smpl_t pitch_found = fvec_get_sample(pitch, 0);
  outmsg("%f %f\n",(blocks)
      *hop_size/(float)samplerate, pitch_found);
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

    buffer_size = 2048;

    char **argtemp;
    examples_common_init(0, argtemp);

    o = new_aubio_pitch (pitch_method, buffer_size, hop_size, samplerate);
    if (pitch_tolerance != 0.)
      aubio_pitch_set_tolerance (o, pitch_tolerance);
    if (silence_threshold != -90.)
      aubio_pitch_set_silence (o, silence_threshold);
    if (pitch_unit != NULL)
      aubio_pitch_set_unit (o, pitch_unit);

    pitch = new_fvec (1);

    wavetable = new_aubio_wavetable (samplerate, hop_size);
    aubio_wavetable_play ( wavetable );
    examples_common_process((aubio_process_func_t)process_block,process_print, (mmp_get_data)convertArray);
}
