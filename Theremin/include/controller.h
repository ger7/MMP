#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "getpitch.h"
#include "notemap.h"
#include "ArmController.h"

class controller
{
public:
    controller();
    void reachAFrequency(float frequency);
    float record(long millisec);
    string findNote(float frequency);
    void moveArmIn();
    void moveArmOut();
    float findFreq(string note);
    float calculateOneDegreeChange();

private:
    Notemap *notes;
    GetPitch *pitch;
    ArmController* ac;
    string robot;
};

#endif // CONTROLLER_H
