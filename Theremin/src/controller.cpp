#include "controller.h"
using namespace std;


controller::controller()
{
    robot= "icub";
    ac = new ArmController(robot);
    notes=new Notemap();
    pitch=new GetPitch();
}

int main(int argc, char* argv[])
{

    controller *control=new controller();
    string theNote= " ";
    float output=0;


    char chWait;
    while(chWait!='e')
    {

        cout<< "Press s to record a sound and analyse the Pitch"<< endl <<
               "Press a to move right arm in by 1 degree and analyse sound" << endl <<
               "Press z to move right arm out by 1 degree and analyse sound"<< endl <<
               "Press f to adjust icubs arms to reach a particular frequency"<< endl <<
               "Press g to make a relative movement based on a recording from the theremin and a recording of a target pitch to reach"<< endl<<
               "Press t to calculate a change in pitch and make a movement based on a note you want to enter"<< endl<<
               "Press p to attempt to play an arpeggio"<<endl<<
               "Press + to increase volume (moves left hand in)"<<endl<<
               "Press - to decrease volume (moves left hand out)"<<endl<<
               "Press x to attempt to play xfiles (much hilarity will ensue)"<<endl<<
               "Press e to exit"<< endl;

        cin >>chWait;
        switch(chWait)
        {
        case 's':
        {
            output=control->record(500);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'a':
        {
            control->moveArmIn();
            output=control->record(500);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'z':
        {
            control->moveArmOut();
            output=control->record(500);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'f':
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
        case 'g':
        {
            cout <<"Making initial recording of Theremin"<< endl;
            float startFreq=control->record(500);
            cout <<"The averaged pitch is: "<< startFreq << endl;
            cout <<"Making second recording in 5 seconds (record the pitch you want to reach)"<< endl;
            usleep(5000000);
            float targetFreq=control->record(500);
            cout <<"The pitch to reach is: "<< targetFreq << endl;
            float result=control->calculateRelativeMove(startFreq, targetFreq);
            cout <<"The amount of degrees to move is: "<< result << endl;
            control->makeCalculatedMove(result);
            break;

        }
        case 't':
        {
            cout<<"Making initial recording"<<endl;
            float startFreq=control->record(500);
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
            //control->makeCalculatedMove(result);
            break;

        }
        case 'p':
        {
            cout<<"Playing Arpeggio"<<endl;
            control->playArpeggio();
            break;
        }
        case 'x':
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
                output=control->record(500);
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
float controller::record(long millisec)
{
    //populate array
    float megaAverage=0;
    float totalAverage=0;
    float averages [16]={};
    float datum;
    int j;
    usleep(1000*millisec);
    for(int i=0; i<16; i++)
    {
        averages [i]=pitch->record(millisec);
        cout<<"Averages [i] is: "<< averages[i]<<endl;
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
    for(int h=0; h<16; h++)
    {
        cout<<"Array contents: "<< averages[h]<<endl;
    }

    //ignore outliers in array
    int outlierIndex = 0;
    int middle=16/2;
    int Q1index = 16 / 4;
    int Q3index = Q1index + middle;
    float Q1ref = averages[Q1index];
    float Q1offref = averages[Q1index-1];
    float Q3ref = averages[Q3index];
    float Q3offref = averages[Q3index-1];
    float Q1=16 % 2 == 0 ? (Q1ref + Q1offref)/2 : Q1ref;
    float Q3=16 % 2 == 0 ? (Q3ref + Q3offref)/2 : Q3ref;

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

void controller::volumeDown()
{
    ac->adjustJoint(LEFT, 5.0);
}

void controller::volumeUp()
{
    ac->adjustJoint(LEFT, -5.0);
}

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
    float prop=diff/(27.0193321126*exp(0.0013133528*startFrequency));
    return prop;
    //float prop=diff/((0.158*startFrequency)+16.920);
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

//Self explanatory function name
void controller::playxFiles()
{
    string tune[]={"A4", "E4", "D4", "E4", "G4", "E4",
                   "A4", "E4", "D4", "E4", "A5", "E4",
                   "C5", "B5", "A5", "G5", "A5", "E5",
                   "C5", "B5", "A5", "G5", "B5", "E5"};

    float startNote=record(300);
    float endNote=findFreq(tune[1]);
    float result =calculateRelativeMove(startNote, endNote);
    makeCalculatedMove(result);
    for(int i=1; i<24; i++)
    {
        startNote=findFreq(tune[i-1]);
        endNote=findFreq(tune[i]);
        float prop=calculateRelativeMove(startNote, endNote);
        usleep(2000000);
        makeCalculatedMove(prop);
    }
}

//Self explanatory function name
void controller::playArpeggio()
{
    string tune[]= {"C4", "F4", "B4"};

    for(int i=0; i<3; i++)
    {
        float startNote=record(500);
        float  endNote=findFreq(tune[i]);
        float prop=calculateRelativeMove(startNote, endNote);
        usleep(1000);
        makeCalculatedMove(prop);
    }
}




