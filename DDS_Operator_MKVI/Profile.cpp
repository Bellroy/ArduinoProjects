//
// Created by Nick on 01-Feb-17.
//

#include "PLL.h"
#include "Profile.h"
#include "Definitions.h"

//Empty Profile Constructor
//Used to set aside memory for a profile instance before it has been [can be] initialized
Profile::Profile() { }

//Main constructor
//Accepts a Profile Register
Profile::Profile(Register &ProfReg) {
    profReg = &ProfReg;
    profReg->read();
    for (int i = 0; i < 8; i++) {
//_data[i]=profReg->_data[i];
        _reg.bytes[i] = profReg->_data[i];
    }
}

//Set the Phase Word in the Profile to the value. Com indicates whether to commit afterwards
void Profile::setPOW(int POW, boolean com) {
    _reg.POW = POW;
    commit(com);
}

//Set the Amplitude Word in the Profile to the value. Com indicates whether to commit afterwards
void Profile::setASF(int ASF, boolean com) {
    _reg.ASF = ASF;
    commit(com);
}

//Set the Frequency Word in the Profile to the value. Com indicates whether to commit afterwards
void Profile::setFTW(unsigned int FTW, boolean com) {
    indexedFTW.word = FTW;
    _reg.FTW = FTW;
//Serial.print("Writing Tuning Word: ");
//Serial.println(FTW);
/* for( int i =4; i<8;i++){
//Serial.println(indexedFTW.bytes[7-i]);
_data[i]=indexedFTW.bytes[7-i];
} */
    if (com) {
        Serial.println("Committing...");
        commit();
    }
}

//Converts a frequency in Hertz into a tuning word
unsigned int Profile::freqToFTW(int Freq) {
//Serial.print("Converting ");
//Serial.println(Freq);
    return (unsigned int) (((long long) (0xFFFFFFFF) * (long long) Freq)
                           / ((long long) PLL::fSysClock));
}

//Sets a frequency in Hertz into the Profile. Com indicates whether to commit
void Profile::setFreq(int Freq, boolean com) {
    Serial.print("Attempting to set to: ");
    Serial.println(Freq);
    unsigned int FTW = freqToFTW(Freq);
    Serial.print("Result of Calculation:");
    Serial.println(FTW);
    setFTW(FTW, com);
}

//Produces the given tuning word at the output of the DDS. Disables all other modes of input
words
void Profile::produceFTW(unsigned int FTW) {
    setFTW(FTW, true);
//Set DRG Disabled, RAM Disabled, Profile Enabled
//RAM
    CFR1._data[0] = (CFR1._data[0] & (0b01111111));
    CFR1.writeReg(CFR1._data);
//DRG
    CFR2._data[1] = (B11110111) & (CFR2._data[1]);
    CFR2.writeReg(CFR2._data);
    update();
}

//Produces the given frequency at the output of the DDS. Disables all other modes of input words
void Profile::produceFreq(int Freq) {
    produceFTW(freqToFTW(Freq));
}

//Commits changes to AD9910 buffer. changes don't take effect until an IO update
void Profile::commit(boolean com) {
    if (com) {
        commit();
    }
}

//Commits changes to AD9910 buffer. changes don't take effect until an IO update
void Profile::commit() {
    profReg->sendWriteAddr();
    profReg->writeBytes(_reg.bytes, 4);
    profReg->writeBytesREndian(_reg.bytes, 4, 7);
}
