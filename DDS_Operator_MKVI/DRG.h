#ifndef DRG_h
#define DRG_h

#include "Arduino.h"
#include "Register.h"

/*DRG class:
This class contains methods and data for controlling the DRG
part of the AD9910 chip
*/
class DRG {
public:
    DRG();

    DRG(int blah);

    void hold();

    void release();

    void slopeControlLow();

    void slopeControlHigh();

    boolean isAtLimit();

    void setDigitalRampDestination(byte value);

    void setDigitalRampEnable(boolean value);

    void commitCFR2();

    void setUpperLimit(unsigned int tuningWord);

    void setLowerLimit(unsigned int tuningWord);

    void commitLimits();

    void setNoDwellHigh(boolean value);

    void setNoDwellLow(boolean value);

    void setNegativeSlope(unsigned short value);

    void setPositiveSlope(unsigned short value);

    void commitSlopes();

    void setPositiveStepRate(unsigned int step);

    void setNegativeStepRate(unsigned int step);

    void commitStepRates();

    void commit();

private:
    union data4 {
        byte b[4];
        unsigned int value;
    };
    union data2 {
        byte b[2];
        short value;
    };
    union data4 _upperLimit;
    union data4 _lowerLimit;
    union data4 _negStep;
    union data4 _posStep;
    union data2 _posSlope;
    union data2 _negSlope;
    byte _CFR1[4];
    byte _CFR2[4];
    byte _DRL[8];
    byte _DRS[8];
    byte _DRR[4];
};

#endif