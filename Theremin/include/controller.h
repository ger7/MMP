#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "getpitch.h"
#include "notemap.h"
#include "ArmController.h"
#include "math.h"

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
    void makeCalculatedMove(float prop);
    float calculateRelativeMove(float startFrequency, float targetFrequency);
    void playxFiles();
    void playArpeggio();
    string findSemitones(string note, int semitones);
    void volumeUp();
    void volumeDown();
    void close();

private:
    Notemap *notes;
    GetPitch *pitch;
    ArmController* ac;
    string robot;
};

#endif // CONTROLLER_H
