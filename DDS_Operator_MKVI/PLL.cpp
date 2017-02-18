//
// Created by Nick on 01-Feb-17.
//

#include "PLL.h"


//Static variable that is set when the PLL is. Contains
//the set clock; used for calculating frequency words
unsigned int PLL::fSysClock;

//Constructor for PLL class object.
//Reads CFR3 data into private data
PLL::PLL() {
    CFR3.read();
    for (int i = 0; i < 4; i++) {
        _CFR3[i] = CFR3._data[i];
        _data.bytes[i] = CFR3._data[i];
    }
}

//Function that sets the DRV0 part of the CFR3 register to the given option
void PLL::setDRV0(byte option) {
    _data.DRV0 = option;
    _CFR3[0] = (option << 4) | ((B11001111) & (_CFR3[0]));
}

//Function that sets the VCO part of the CFR3 register to the given value
void PLL::setVCO(int value) {
    _data.VCO = value;
    _CFR3[0] = (value) | ((B11111000) & (_CFR3[0]));
}

//Sets the PLL multiplier section of CFR3 to the given multiplier
void PLL::setN(byte multiplier) {
    _data.N = multiplier;
    _CFR3[3] = (multiplier << 1) | ((B00000001) & (_CFR3[3]));
}

//Sets or unsets the PLL enable bit in CFR3
void PLL::setPLLEnable(boolean value) {
    _data.PLL_Enable = value;
    _CFR3[2] = (value) | ((B11111110) & (_CFR3[2]));
}

//Sets or unsets the Divider Bypass bit in CFR3
void PLL::setDividerBypass(boolean value) {
    _data.InDivBypass = value;
    _CFR3[2] = (value << 7) | ((B01111111) & (_CFR3[2]));
}

//Sets or unsets the Divder Bypass Reset bit in CFR3
void PLL::setDividerResetB(boolean value) {
    _data.InDivBypassB = value;
    _CFR3[2] = (value << 6) | ((B10111111) & (_CFR3[2]));
}

//Sets or unsets the Set PFD Reset bit in CFR3
void PLL::setPFDReset(boolean value) {
    _data.PFD_Reset = value;
    _CFR3[2] = (value << 2) | ((B11111011) & (_CFR3[2]));
}

//Sets the output current section of CFR3 to the given value
void PLL::setOutputCurrent(byte value) {
    _data.Icp = value;
    _CFR3[1] = (value << 3) | ((B11000111) & (_CFR3[3]));
}

//Returns true if PLL lock is established, false otherwise
boolean PLL::isLocked() {
    return digitalRead(PLL_LOCK);
}

//Static function that converts a frequency (in Hz) to a tuning word based on the current clock
unsigned int PLL::freqToFTW(int Freq) {
//Serial.print("Converting ");
//Serial.println(Freq);
    return (unsigned int) (
            ((long long) (0xFFFFFFFF) * (long long) Freq)
            / ((long long) PLL::fSysClock));
}

//Function that sets all needed PLL parameters based on an input clock and desired output
void PLL::quickSetup(int inMHz, int outMHz) {
    PLL::fSysClock = outMHz * 1000000;
    if (outMHz > 200) {
        fastUpdate = true;
    }
    Serial.print("Setting PLL ");
    Serial.print(inMHz);
    Serial.print(" MHz -> ");
    Serial.print(outMHz);
    Serial.println(" MHz");
    setN(outMHz / inMHz);
    if (outMHz >= 880) {
        setVCO(5);
    }
    else if (outMHz >= 770) {
        setVCO(4);
    }
    else if (outMHz >= 650) {
        setVCO(3);
    }
    else if (outMHz >= 550) {
        setVCO(2);
    }
    else if (outMHz >= 450) {
        setVCO(1);
    }
    else {
        setVCO(0);
    }
    setPLLEnable(true);
    commit();
}

//Save all changes to CFR3 to buffer. Changes do not take effect until in IO Update
void PLL::commit() {
    CFR3.writeReg(_CFR3);
}
