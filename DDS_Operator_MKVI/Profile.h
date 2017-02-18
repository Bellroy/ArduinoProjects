//Library that defines getting a single-tone frequency output from the AD9910
#ifndef Profile_h
#define Profile_h

#include "Arduino.h"
#include "Register.h"

class Profile {
public:
    Profile();

    Profile(Register &ProfReg);

    void setFTW(unsigned int FTW, boolean com = false);

    static unsigned int freqToFTW(int Freq);

    void setFreq(int freq, boolean com = true);

    void setPOW(int POW, boolean com = false);

    void setASF(int ASF, boolean com = false);

    void produceFreq(int Freq);

    void produceFTW(unsigned int FTW);

    void commit();

private:
    void commit(boolean com);

    union i_ftw {
        unsigned int word;
        byte bytes[4];
    } indexedFTW;
    Register *profReg;
    byte _data[8];
    union {
        struct {
            unsigned int :2;
            unsigned short ASF :14;
            unsigned short POW :16;
            unsigned int FTW;
        };
        byte bytes[8];
    } _reg;
};

#endif