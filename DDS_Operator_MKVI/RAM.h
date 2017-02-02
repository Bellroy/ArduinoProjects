//
// Created by Nick on 01-Feb-17.
//

//Class that controls writing to DDS RAM
#ifndef RAM_h
#define RAM_h

#include "Arduino.h"
#include "Register.h"

#define FREQUENCY 0
#define PHASE 32
#define AMPLITUDE 64
#define POLAR 96

class RAM {
public:
    RAM();

    RAM(Register &cfr1, Register &prof0, Register &ramWrite);

    void stallFreq(unsigned int FTW);

    void stopStall();

    void setupStall(unsigned int FTW);

    void setSingleTone();

private:
    void setMode(byte mode);

    void writeWord(unsigned int word);

    void setRAMEnabled();

    void setRAMDisabled();

    void commitCFR1();

    void totalCommit();

    union {
        struct {
            unsigned int WORD;
        };
        byte bytes[8];
    } _RAMreg;
    union {
        struct {
            int :8;
            int addStepRate :16;
            int end_address :10;
            int :6;
            int start_address:10;
            int :8;
            int noDwell :1;
            int :1;
            int zeroCross :1;
            int ramMode :3;
        };
        byte bytes[8];
    } _profReg;
    byte currentMode;
    Register *CFR1;
    Register *Prof0;
    Register *RAMWrite;
};

#endif