#include "notemap.h"
using namespace std;

const int arraySize=120;

//Frequencies in Hz for C0 - B9
const float freq []={
    16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87,
    32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74,
    65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47,
    130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
    261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
    523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,
    1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53,
    2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07,
    4186.01, 4434.92, 4698.63, 4978.04, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.14, //C8 is highest note on piano i.e the value at the beginning of this line
    8372.02, 8869.85, 9397.28, 9956.07, 10548.08, 11175.30, 11839.82, 12543.86, 13289.76, 14080.01, 14917.25, 15804.27};  //shouldnt really go this high

//String vlaues for notes from C0 - B9
const string notes []={
    "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
    "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
    "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
    "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9", "G#9", "A9", "A#9", "B9" };


Notemap::Notemap()
{

}


//Returns a note from the list of notes after searching through the list of frequencies for the closest frequency to the one supplied
//by the controller.
string Notemap::findNote(float f)
{
    string theNote= "";
    for(int i=0; i<arraySize; i++)
    {
        float currentHz=freq[i];
        float nextHz=freq[i+1]; //index out of bounds
        if(f<nextHz&&f>currentHz)
        {
            float distfromcurrent=f-currentHz;
            float distfromnext=nextHz-f;

            if(distfromcurrent<distfromnext)
            {
                f=currentHz;
                theNote= notes[i];
                break;
            }

            else
            {
                f=nextHz;
                theNote= notes[i+1];
                break;
            }
        }
    }
    return theNote;
}

//Returns the frequency after specifying a note to return the frequency of
float Notemap::findFreq(string note)
{
    float frequency= 0;
    for(int i=0; i<arraySize; i++)
    {
        if(note==notes[i])
        {
            frequency=freq[i];
            break;
        }
    }
    return frequency;
}

string Notemap::findSemitones(string note, int semitones) //returns the note thats however many semitones away (chosen by user) from the note (chosen by user)
{
    string result;
    for(int i=0; i<arraySize; i++)
    {
        if(note==notes[i])
        {
            if(i+semitones<arraySize)
            {
                result=notes[i+semitones];
                break;
            }
        }
    }
    return result;
}

//Finds the number of semitones between two given notes
int Notemap::findNumberOfSemitones(string note, string note2)
{

    int semitones;
    int indexnote1;
    int indexnote2;

    for(int i=0; i<arraySize; i++)
    {
        if(note==notes[i])
        {
            indexnote1=i;
            break;
        }
    }

    for(int j=0; j<arraySize; j++)
    {
        if(note2==notes[j])
        {
            indexnote2=j;
            break;
        }
    }

    semitones=indexnote2-indexnote1;
    return semitones;

}

//TODO - better averaging for the above?

//Always doubles every octave - perhaps halving the distance between the iCub's hand
//and the theremin will produce a sound one octave higher? Worth noting.
