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
               "Press g to calculate a change of pitch to one degree of movement"<< endl<<
               "Press e to exit"<< endl;

        cin >>chWait;
        switch(chWait)
        {
        case 's':
        {
            output=control->record(3000);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'a':
        {
            control->moveArmIn();
            output=control->record(1000);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=control->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'z':
        {
            control->moveArmOut();
            output=control->record(1000);
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
            cout<<"The frequency found is: "<< foundFreq << endl;
            control->reachAFrequency(foundFreq);
            break;

        }
        case 'g':
        {
        control->calculateOneDegreeChange();
        }
        case 'e':
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Unrecognised input" << endl;
            break;
        }
    }
}

float controller::record(long millisec)
{
   return pitch->record(millisec);
}

float controller::findFreq(string note)
{
    return notes->findFreq(note);
}

string controller::findNote(float frequency)
{
  return notes->findNote(frequency);
}

void controller::moveArmIn()
{
    ac->adjustJoint(RIGHT, -1.0);
}

void controller::moveArmOut()
{
    ac->adjustJoint(RIGHT, 1.0);
}


void controller::reachAFrequency(float frequency)
{
    float currentfreq=record(1000);
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
        currentfreq=record(1000);
        cout<<"The current frequency is: " << currentfreq <<endl;
    }

    cout << "Reached frequency successfully, the pitch is: " << currentfreq << " and is " << (frequency-currentfreq) << " away from the exact frequency."<<endl;

}

float controller::calculateOneDegreeChange()
{
    float initFreq=record(1000);
    float result=0;

    moveArmIn();
    float nextFreq=record(1000);

    result=nextFreq-initFreq;
    cout<<"Change was :" << result<< endl;
    return result;
}


