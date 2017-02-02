//
// Created by Nick on 01-Feb-17.
//

//PLL class for handling the Phase-Locked-Loop functions of the AD9910
#ifndef PLL_h
#define PLL_h

#include "Arduino.h"
#include "Register.h"

class PLL {
public:
    PLL();

    void setDRV0(byte option);

    void setVCO(int value);

    void setN(byte multiplier);

    void setPLLEnable(boolean value);

    void setInputDividerBypass(boolean value);

    void setInputDividerResetB(boolean value);

    void setDividerBypass(boolean value);

    void setDividerResetB(boolean value);

    void setPFDReset(boolean value);

    static unsigned int freqToFTW(int Freq);

    void setOutputCurrent(byte option);

    boolean isLocked();

    void commit();

    void quickSetup(int inMHz, int outMHz);

    static unsigned int fSysClock;
private:
//Register *CFR3;
    byte _CFR3[4];
    union {
        struct {
            unsigned int :2;
            unsigned int DRV0 : 2;
            unsigned int :1;
            unsigned int VCO :3;
            unsigned int :2;
            unsigned int Icp :3;
            unsigned int :3;
            unsigned int InDivBypass: 1;
            unsigned int InDivBypassB: 1;
            unsigned int :3;
            unsigned int PFD_Reset: 1;
            unsigned int :1;
            unsigned int PLL_Enable: 1;
            unsigned int N :7;
            unsigned int :1;
        };
        byte bytes[4];
    } _data;
};

#endif
