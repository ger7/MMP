#include "getpitch.h"


GetPitch* myPitch;


GetPitch::GetPitch()
{
    set=false;
    objAudioBuffer = AudioBuffer((int) (SAMPLE_RATE * 60));

    myPitch = this;
}



float GetPitch::record(long millisec)
{
    int 	iInputDevice = -1;
    portaudio::AutoSystem autoSys;  //initialises system instance
    portaudio::System &sys = portaudio::System::instance();
    iInputDevice	= sys.defaultInputDevice().index();

    cout << "Opening recording input stream on " << sys.deviceByIndex(iInputDevice).name() << endl;
    portaudio::DirectionSpecificStreamParameters inParamsRecord(sys.deviceByIndex(iInputDevice), 1, portaudio::INT16, false, sys.deviceByIndex(iInputDevice).defaultLowInputLatency(), NULL);
    portaudio::StreamParameters paramsRecord(inParamsRecord, portaudio::DirectionSpecificStreamParameters::null(), SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);
    portaudio::MemFunCallbackStream<AudioBuffer> streamRecord(paramsRecord, objAudioBuffer, &AudioBuffer::RecordCallback);

    streamRecord.start();
    cout << "Starting to record" << endl;
    usleep(millisec*1000); //usleep sleeps for a value in microseconds, hence *1000
    cout << endl << "Stopping recording" << endl;
    streamRecord.stop();
    objAudioBuffer.ResetPlayback();

    //AUBIO PITCH PROCESSING -- gets the pitch of the above recorded sound
    cout << "Initalising Aubio" << endl;
    examples_common_init();

    cout << "Processing data to calculate pitch" << endl;
    float standardPitch=examples_common_process((mmp_get_data)convertArray);
    return standardPitch;
}


int convertArray(fvec_t * ibuf, uint_t hop_size)
{
    int hasData = 0;        //hasData dictates whether there is still data left in the array and will stop the program from iterating too many times/running too long
    if(!myPitch->set)
    {
        myPitch->it = myPitch->objAudioBuffer.getIterator();
        myPitch->vec = myPitch->objAudioBuffer.getVectors();
        myPitch->set = true;
    }

    int count =0;

    VECTOR_SHORT_ITER itLocal = myPitch->it;



    for(uint_t i=0;i<hop_size;i++)
    {
        float pitchdata = 0.0;
        if(itLocal!=myPitch->vec.end())
        {

            pitchdata=(float)(*itLocal)/10000.00; //converts data so that it can be used in arrays for pitch detection in aubio
            itLocal++;
            hasData=1;
        }
        else
        {
            hasData=0;
        }

        //checking to make sure that when the end of the data is reached i.e. reads zeros in the array that it stops the do loop in the aubio pitch detection file
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

        myPitch->it = itLocal;

    }
    return hasData;
}
