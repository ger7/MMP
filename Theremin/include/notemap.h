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
    int findNumberOfSemitones(std::string note, std::string note2);
};

#endif // NOTEMAP_H
