#include "controller.h"
using namespace std;


controller::controller()
{
    robot= "icub";
    ac = new ArmController(robot);
    notes=new Notemap();
    pitch=new GetPitch();
}

controller::controller(bool hasYarp)
{
    armControlOn=hasYarp;
    robot= "icub";
    if(hasYarp)
    {
        ac = new ArmController(robot);
    }
    notes=new Notemap();
    pitch=new GetPitch();

}

int main(int argc, char* argv[])
{
    controller *control=new controller(true);
    string theNote= " ";
    float output=0;


    char chWait;
    while(chWait!='e')
    {

        cout<< "Press s to record a sound and analyse the Pitch"<< endl <<
               "Press a to move right arm in by 1 degree and analyse sound" << endl <<
               "Press z to move right arm out by 1 degree and analyse sound"<< endl <<
               "Press f to adjust icubs arms to reach a particular frequency incrementally"<< endl <<
               "Press g to make a relative movement based on a recording from the theremin and a recording of a target pitch to reach"<< endl<<
               "Press t to calculate a change in pitch and make a movement based on a note you want to enter"<< endl<<
               "Press p to attempt to play an arpeggio"<<endl<<
               "Press l to record the current frequency, then 1 second after record another and make a movement!"<<endl<<
               "Press o to calculate the note that is x semitones (specified by user) away from either a recorded note or user specified note"<< endl<<
               "Press + to increase volume (moves left hand in)"<<endl<<
               "Press - to decrease volume (moves left hand out)"<<endl<<
               "Press x to attempt to play xfiles (much hilarity will ensue)"<<endl<<
               "Press e to exit"<< endl;

        cin >>chWait;
        switch(chWait)
        {
        case 's': //records a sound and analyses its frequency
        {
            output=control->record(300);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'a': //moves the right arm in by 1 degree and analyses the frequency produced
        {
            control->moveArmIn();
            output=control->record(300);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'z': //moves the right arm out by one degree and analyses the frequency produced
        {
            control->moveArmOut();
            output=control->record(300);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'f': //incrementally reaches a note through 1 degree movements to get as close to the pitch required as it can
        {
            cout<<"Enter a note you want to reach: "<< endl;
            string input;
            cin>>input;
            float foundFreq=control->findFreq(input);
            if(foundFreq==0)
            {
                cout<<"Didnt find note"<< endl;
                break;
            }
            cout<<"The frequency to reach is: "<< foundFreq << endl;
            control->reachAFrequency(foundFreq);
            break;
        }
        case 'g': //records a pitch, then records a pitch you wish to reach, then makes the calculated move necessary for the robot
        {
            char input;
            cout <<"Making initial recording of Theremin"<< endl;
            float startFreq=control->record(300, 3, true);
            cout <<"The averaged pitch is: "<< startFreq << endl;
            string note=control->findNote(startFreq);
            cout<<"The note for initial recording is: "<< note<< endl;
            cout <<"Making second recording when you press r (record the pitch you want to reach)"<< endl;
            cin>>input;
            if(input=='r')
            {
                float targetFreq=control->record(300, 3, true);
                cout <<"The pitch to reach is: "<< targetFreq << endl;
                note=control->findNote(targetFreq);
                cout<<"The note to reach is: "<< note <<endl;
                float result=control->calculateRelativeMove(startFreq, targetFreq);
                cout <<"The amount of degrees to move is: "<< result << endl;
                control->makeCalculatedMove(result);
                break;
            }
            else
            {
                cout<< "input not recognised press r to record second recording"<<endl;
                break;
            }
            break;

        }
        case 't': //Specifies a starting frequency, then prompts user to enter a note to reach. After this, makes a calculation for the relative move needed and makes the move
        {
            cout<<"Making initial recording"<<endl;
            float startFreq=control->record(300);
            cout <<"The averaged pitch is: "<< startFreq << endl;
            theNote=control->findNote(startFreq);
            cout <<"The note was roughly: "<< theNote<< endl;
            cout<<"Enter a note you want to reach: "<< endl;
            string input;
            cin>>input;
            float targetFreq=control->findFreq(input);
            if(targetFreq==0)
            {
                cout<<"Didnt find note"<< endl;
                break;
            }
            cout<<"The frequency to reach is: "<< targetFreq << endl;
            float result =control->calculateRelativeMove(startFreq, targetFreq);
            cout<< "The amount of degrees to move is: "<< result <<endl;
            control->makeCalculatedMove(result);
            break;

        }
        case 'l': //builds a tune then attempts to play tune based on recording
        {
            cout<<"Making initial recording in 3 seconds"<<endl;
            usleep(1000000);
            cout<<"READY"<<endl;
            usleep(1000000);
            cout<<"SET"<<endl;
            usleep(1000000);
            cout<<"GO"<<endl;
            vector <float> v=control->buildTune();
            cout<<"Move the microphone to the icub and press y to continue"<<endl;
            char input;
            cin>>input;
            if(input=='y')
            {
            control->playTune(v);
            }
            else
            {
            break;
            }
            break;
        }
        case 'p': //attempts to play an arpeggio
        {
            cout<<"Playing Arpeggio"<<endl;
            control->playArpeggio();
            break;
        }
        case 'x': //attempts to play the X files theme tune
        {
            control->playxFiles();
            break;
        }
        case 'o':
        {
            char answer;
            string note;
            float output;
            string result;
            int semi;
            cout<<"Do you want to specify a note yourself? Press y for yes"<<endl;
            cin>>answer;
            if(answer!='y')
            {
                output=control->record(300);
                note=control->findNote(output);
                cout<<"Specify the amount of semitones away you want to go"<<endl;
                cin>>semi;
                result=control->findSemitones(note, semi);
                cout<<"The note "<< semi<< " semitones away from "<< note<< "is "<<result<<endl;
                break;
            }
            else if(answer=='y')
            {
                cout<<"Specify the note you want to reach:"<<endl;
                cin>>note;
                cout<<"Specify the amount of semitones away you want to go"<<endl;
                cin>>semi;
                result=control->findSemitones(note, semi);
                cout<<"The note "<< semi<< " semitones away from "<< note<< "is "<<result<<endl;
                break;
            }
            else
            {
                cout<<"Sorry, input was not recognised"<<endl;
                break;
            }
            break;
        }
        case 'e':
            cout << "Exiting..." << endl;
            control->close();
            break;
        default:
            cout << "Unrecognised input" << endl;
            break;
        }
    }
}

//Records a peice of data and returns its average pitch in Hz. Records for an amount of time specified in milliseconds. Actual function in getpitch class.
//Edited to return an average of the averaged pitches for better accuracy. Also removes outliers from set of samples.
float controller::record(long millisec, int repeats, bool silent)
{
    //populate array
    float megaAverage=0;
    float totalAverage=0;
    float* averages=new float [repeats];
    float datum;
    int j;
    usleep(1000*millisec);
    for(int i=0; i<repeats; i++)
    {
        averages [i]=pitch->record(millisec);
        if(!silent)
        {
        cout<<"Averages [i] is: "<< averages[i]<<endl;
        }
        for (int k = 1; k <= i; ++k)
        {
            datum = averages[k];
            for (j = k; j > 0; j--)
            {
                float nextValue=averages[j-1];
                if(nextValue>datum)
                    averages[j]=nextValue;
                else break;
            }
            averages[j]=datum;
        }


    }
    for(int h=0; h<repeats; h++)
    {
        if(!silent)
        {
        cout<<"Array contents: "<< averages[h]<<endl;
        }
    }

    //ignore outliers in array
    int outlierIndex = 0;
    int middle=repeats/2;
    int Q1index = repeats / 4;
    int Q3index = Q1index + middle;
    float Q1ref = averages[Q1index];
    float Q1offref = averages[Q1index-1];
    float Q3ref = averages[Q3index];
    float Q3offref = averages[Q3index-1];
    float Q1=repeats % 2 == 0 ? (Q1ref + Q1offref)/2 : Q1ref;
    float Q3=repeats % 2 == 0 ? (Q3ref + Q3offref)/2 : Q3ref;

    float iqRangeInner=(Q3-Q1)*1.5;
    float innerFence1= Q1-iqRangeInner; //For mild outliers
    float innerFence2= Q3+iqRangeInner;

    while(averages[outlierIndex]<innerFence1)
        outlierIndex++;
    while(averages[outlierIndex]==0)
    {
        outlierIndex++;
    }
    int start = outlierIndex;
    while(averages[outlierIndex]<innerFence2)
    {
        if(averages[outlierIndex]==0)
            break;
        outlierIndex++;
    }
    int end = outlierIndex-1;

    //calculate average
    for(int j=start; j<end; j++)
    {
        totalAverage+=averages[j];
    }

    megaAverage=totalAverage/(end-start);
    return megaAverage; //return average of averages with outliers ignored
}

//Returns a frequency when given a note as the parameter - actual function in Notemap class
float controller::findFreq(string note)
{
    return notes->findFreq(note);
}

//Find the number of semitones between two given notes
int controller::findNumberOfSemitones(string note, string note2)
{
    return notes->findNumberOfSemitones(note, note2);
}

//Returns a note when given a particular frequency - actual  function in Notemap class
string controller::findNote(float frequency)
{
    return notes->findNote(frequency);
}

//Moves the arm in by 1 degree - actual method in ArmController class
void controller::moveArmIn()
{
    ac->adjustJoint(RIGHT, -1.0);
}

//Moves the arm out by 1 degree - actual method in ArmController class
void controller::moveArmOut()
{
    ac->adjustJoint(RIGHT, 1.0);
}

//Moves left arm outwards, lowering the volume
void controller::volumeDown()
{
    ac->adjustJoint(LEFT, 5.0);
}

//Moves left arm inwards, raising the volume
void controller::volumeUp()
{
    ac->adjustJoint(LEFT, -5.0);
}

//Shuts down the icub safely using predefined method from ArmController class
void controller::close()
{
    ac->close();
}

//Returns a note thats however many semitones away from a "starting note" that the user chooses. Specified in Notemap class
string controller::findSemitones(string note, int semitones)
{
    return notes->findSemitones(note, semitones);
}

//Reaches a particular frequency by incrementing the movement of the arm by one degree at a time depending on the recording
void controller::reachAFrequency(float frequency)
{
    float currentfreq=record(500);
    float boundry=frequency*0.02;


    while(currentfreq<(frequency-boundry) || currentfreq>(frequency+boundry))
    {
        if(currentfreq<frequency)
        {
            ac->adjustJoint(RIGHT, -1.0);
        }
        else
        {
            ac->adjustJoint(RIGHT, 1.0);
        }
        currentfreq=record(500);
        cout<<"The current frequency is: " << currentfreq <<endl;
    }

    cout << "Reached frequency successfully, the pitch is: " << currentfreq << " and is " << (frequency-currentfreq) << " away from the exact frequency."<<endl;
}

//Calculates the amount of Hz change one degree of movement has made.
float controller::calculateOneDegreeChange()
{
    float initFreq=record(500);
    float result=0;

    moveArmIn();


    float nextFreq=record(500);

    result=nextFreq-initFreq;
    cout<<"Change was :" << result<< endl;
    return result;
}

//Calculates a proportion of relative motor movement to pitch change, which I have garnered through testing. First, the difference between the actual frequency and the
//target frequncy is calculated, this is then divided by the f(x) (or line equation) of data from testing. The result is the amount of degrees the robot will move its arm by
float controller::calculateRelativeMove(float startFrequency, float targetFrequency)
{
    float diff=startFrequency-targetFrequency;
    float prop=diff/(28.4290165697 *exp(0.0013271426*startFrequency));
    cout<<"Movement of: "<<prop<<" needed"<<endl;
    return prop;
    //float prop=diff/(27.0193321126*exp(0.0013133528*startFrequency));
}

//Makes a calculated move of the robots arm using the proportion calculated in the function above as a parameter.
void controller::makeCalculatedMove(float prop)
{
    if(prop>10)
    {
        cout<<"Value for prop greater than 10 moving 10 to compensate"<< endl;
        ac->adjustJoint(RIGHT, 10);
    }
    else if(prop<-10)
    {
        cout<<"Value for prop less than -10 moving -10 to compensate"<< endl;
        ac->adjustJoint(RIGHT, -10);
    }

    ac->adjustJoint(RIGHT, prop);
}

//Builds a vector of notes of a recorded tune from keyboard
vector <float> controller::buildTune()
{
    vector <float> tune;
    tune.reserve(10);

    float hz=251;

    while(hz>250)
    {
        cout<<"PLAY"<<endl;
        hz=record(200, 5, true);
        //usleep(200000);

        if(hz>250)
        {
        tune.push_back(hz);
        }
    }

    return tune;
}

//Plays the vector of Notes that have been built
void controller::playTune(vector <float> tune)
{
    cout<<"Vector size is: "<<tune.size() <<endl; //prints for testing
    cout<<""<<endl;
    cout<<"Move microphone to theremin speakers"<<endl;
    usleep(2000000);
    float firstnote=record(200, 5, true); //initial recording for first move from the icubs current position
    float secondnote= tune.at(0);
    float result=calculateRelativeMove(firstnote, secondnote);
    makeCalculatedMove(result);

    for(int i=1; i<tune.size(); i++)
    {
        cout<<"Vector contents are: "<< tune.at(i)<<endl;
        cout<<"Note for above frequency is: "<< findNote(tune.at(i))<< endl; //more testing prints

        firstnote=tune[i-1];
        secondnote=tune[i];
        result=calculateRelativeMove(firstnote, secondnote);
        makeCalculatedMove(result);
    }

}




//Takes in current frequency (recorded and passed to param), the records a second frequency and tries to move to match it
void controller::playRecordedNote(float currentfrequency)
{
    currentfrequency=pitch->record(300);
    cout<<"Making Second recording in one second"<<endl;
    usleep(300000);
    float endHz=pitch->record(200);
    float result=calculateRelativeMove(currentfrequency, endHz);
    //makeCalculatedMove(result);
}


//Self explanatory function name
void controller::playxFiles()
{
    string tune[]={"A4", "E4", "D4", "E4", "G4", "E4",
                   "A4", "E4", "D4", "E4", "A5", "E4",
                   "C5", "B5", "A5", "G4", "A5", "E4",
                   "C5", "B5", "A5", "G4", "B5", "E4"};

    float startHz=record(300);
    string startNote=findNote(startHz);                     //starting note for theremin
    string endNote="A4";                                    //A4
    float endHz=findFreq(endNote);
    float result =calculateRelativeMove(startHz, endHz);    //Move to A4
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 7);    //E5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //D5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 2);                    //E5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 3);                    //G5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -3);                   //E5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    usleep(2000000);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -7);                   //A4
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 7);                    //E5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //D5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 2);                    //E4
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 5);                    //A5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -5);                   //E4
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    usleep(2000000);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 8);                    //C5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -1);                   //B5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //A5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //G4
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 2);                    //A5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -5);                   //E4
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    usleep(2000000);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 8);                    //C6
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -1);                   //B5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //A5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -2);                   //G5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 4);                    //B5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -7);                   //E5
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
    cout<<startNote<<endl;

    cout<<"Fin"<<endl;

}

//Self explanatory function name
void controller::playArpeggio()
{
    float startHz=record(300);
    string startNote=findNote(startHz);
    string  endNote=findSemitones(startNote, 4);
    float endHz=findFreq(endNote);
    float result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 3);
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, 6);
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -6);
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -3);
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);

    startNote=endNote;
    startHz=endHz;
    endNote=findSemitones(startNote, -4);
    endHz=findFreq(endNote);
    result=calculateRelativeMove(startHz, endHz);
    makeCalculatedMove(result);
}




