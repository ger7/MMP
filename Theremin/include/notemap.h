#ifndef NOTEMAP_H
#define NOTEMAP_H
#include <string>

class Notemap
{


public:
    Notemap();
    std::string findNote(float f);
    float findFreq(std::string note);
    std::string findSemitones(std::string note, int semitones);
};

#endif // NOTEMAP_H
