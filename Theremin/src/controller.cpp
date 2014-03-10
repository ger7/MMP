#include "controller.h"

controller::controller()
{

}

int main(int argc, char* argv[])
{
    char chWait;
    cout<<"Press Enter to analyse 1 second of sound";
    cin.get(chWait);
    GetPitch *pitch=new GetPitch();
    float output= pitch->record(1000);
    cout <<"The averaged pitch is: "<< output << endl;
}
