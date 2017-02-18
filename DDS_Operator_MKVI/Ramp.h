//Ramp object. Contains data needed to make a Ramp
//Should take up 17 bytes
#ifndef Ramp_h
#define Ramp_h

#include "Arduino.h"

class Ramp {
public:
    Ramp(unsigned int lowWord, unsigned int highWord, unsigned int timeStep, unsigned int
    wordStep, boolean noDwellHigh, boolean noDwellLow, byte wordDest, boolean dir);

    Ramp(int lowFreq, int highFreq, long long milliseconds, byte wordDest, boolean dir = 1);

    Ramp();

    struct {
        unsigned int lowWord;
        unsigned int highWord;
        unsigned int timeStep;
        unsigned int wordStep;
        unsigned int noDwellHigh :1;
        unsigned int noDwellLow : 1;
        unsigned int wordDest :3;
        unsigned int direction: 1;
        unsigned int :1;
    };
private:
};

#endif