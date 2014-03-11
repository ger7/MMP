#include "controller.h"
using namespace std;
ArmController* ac;

controller::controller()
{

}

int main(int argc, char* argv[])
{
    string robot = "icub";
    ac = new ArmController(robot);
    Notemap *notes=new Notemap();
    GetPitch *pitch=new GetPitch();

    string theNote= " ";
    float output=0;

    char chWait;
    while(chWait!='e')
    {

        cout<< "Press s to record a sound and analyse the Pitch"<< endl <<
               "Press a to move right arm in by 1 degree and analyse sound" << endl <<
               "Press z to move right arm out by 1 degree and analyse sound"<< endl <<
               "Press e to exit"<< endl;

        cin >>chWait;
        switch(chWait)
        {
        case 's':
        {
            output= pitch->record(3000);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=notes->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'a':
        {
            ac->adjustJoint(RIGHT, 1.0);
            output= pitch->record(1000);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=notes->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
        }
        case 'z':
        {
            ac->adjustJoint(RIGHT, -1.0);
            output= pitch->record(1000);
            cout <<"The averaged pitch is: "<< output << endl;
            theNote=notes->findNote(output);
            cout<<"The note received was roughly a: "<< theNote << endl;
            break;
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
