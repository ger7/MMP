#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "getpitch.h"
#include "notemap.h"
#include "ArmController.h"
#include "math.h"
#include <vector>

class controller
{
public:
    controller();
    controller(bool hasYarp);
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
    void playRecordedNote(float currentfrequency);
    int findNumberOfSemitones(string note, string note2);
    std::vector<float> buildTune();
    void playTune(std::vector<float> tune);

private:
    Notemap *notes;
    GetPitch *pitch;
    ArmController* ac;
    string robot;
    bool armControlOn;
};

#endif // CONTROLLER_H
