//
// Created by Nick on 01-Feb-17.
//


#include "Ramp.h"

//Returns empty(initialized to 0) ramp object
Ramp::Ramp() { }

//Places data into fields
Ramp::Ramp(unsigned int lowWord, unsigned int highWord, unsigned int timeStep,
           unsigned int wordStep, boolean noDwellHigh,
           boolean noDwellLow, byte wordDest, boolean dir) {
    lowWord = lowWord;
    highWord = highWord;
    timeStep = timeStep;
    wordStep = wordStep;
    noDwellHigh = noDwellHigh;
    noDwellLow = noDwellLow;
    wordDest = wordDest;
    direction = dir;
}

//Converts readable data and places into fields (Slowest)
Ramp::Ramp(int lowFreq, int highFreq, long long milliseconds, byte wordDest, boolean dir) {
    lowWord = PLL::freqToFTW(lowFreq);
    highWord = PLL::freqToFTW(highFreq);
    direction = dir;
    int count = 0;
    timeStep = 0;
    while (timeStep == 0) {
        count++;
        timeStep = ((count * milliseconds * ((long long) (PLL::fSysClock))) / (highWord - lowWord)) / 4000;
    }
    wordDest = wordDest;
    noDwellHigh = 0;
    noDwellLow = 0;
    if (wordDest == 0) {
//Frequency word
        wordStep = count;
//wordStep = ((unsigned long long)timeStep * PLL::fSysClock)/(0x100000000);
    }
}