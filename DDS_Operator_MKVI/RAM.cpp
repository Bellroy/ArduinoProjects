//
// Created by Nick on 01-Feb-17.
//

#include "RAM.h"


//Blank constructor. Sets aside memory for object without initializing it
RAM::RAM() { }

//Main Constructer for a Ram object. Intializes to SingeTone mode
RAM::RAM(Register &cfr1, Register &prof0, Register &ramWrite) {
    CFR1 = &cfr1;
    Prof0 = &prof0;
    RAMWrite = &ramWrite;
    currentMode = 0; //Current mode is 0 (Frequency)
//Read bytes into private storage
    for (int i = 0; i < 8; i++) {
        _profReg.bytes[i] = 0;
    }
//Set RAM mode to single tone mode
    setSingleTone();
}

//Sets the Ram to single Tone mode
void RAM::setSingleTone() {
    _profReg.ramMode = 0;
    _profReg.noDwell = 0;
    _profReg.zeroCross = 0;
    _profReg.start_address = 0;
//_profReg.end_address=0;
    _profReg.bytes[4] = 0b01000000;
    Prof0->writeReg(_profReg.bytes);
    update();
}

//Sets the word destinition mode to the given value
void RAM::setMode(byte mode) {
    currentMode = mode;
//Serial.print("Setting mode:\nBefore:");
//Serial.println(CFR1->_data[0]);
    CFR1->_data[0] = ((CFR1->_data[0] & (0b10011111)) | mode);
//Serial.print("After:");
//Serial.println(CFR1->_data[0]);
}

//Enables the RAM in the CFR1 Register. When RAM is enabled, it overrides all other word sources
void RAM::setRAMEnabled() {
//Serial.print("Setting Ram Enabled:\nBefore:");
//Serial.println(CFR1->_data[0]);
    CFR1->_data[0] = ((CFR1->_data[0] & (0b01111111)) | (0b10000000));
//Serial.print("After:");
    //Serial.println(CFR1->_data[0]);
}

//Set the RAM enable bit to disabled in the CFR1 register. The RAM will cease to be a word source
void RAM::setRAMDisabled() {
    CFR1->_data[0] = (CFR1->_data[0] & (0b01111111));
}

//Writes a FTW into the RAM.
void RAM::writeWord(unsigned int word) {
    _RAMreg.WORD = word;
//Serial.print("Writing ");
//Serial.print(_RAMreg.WORD);
//Serial.println(" to RAM");
    RAMWrite->sendWriteAddr();
    RAMWrite->writeBytesREndian(_RAMreg.bytes, 4, 3);
    RAMWrite->writeBytesREndian(_RAMreg.bytes, 4, 3);
}

//Commit changes to CFR1 to the buffer. Changes do not take effect until an update
void RAM::commitCFR1() {
    CFR1->writeReg(CFR1->_data);
}

//Commits CFR1 and then performs an update
void RAM::totalCommit() {
    commitCFR1();
    update();
//CFR1->readout();
}

//Sets up a RAM stall at the given FTW
void RAM::setupStall(unsigned int FTW) {
    if (currentMode != FREQUENCY) {
//Serial.println("Mode not in Frequency; Setting...");
        setMode(FREQUENCY);
    }
    writeWord(FTW);
    setRAMEnabled();
    commitCFR1();
}

//Performs a stall at the given FTW
void RAM::stallFreq(unsigned int FTW) {
    setupStall(FTW);
    update();
}

//Stops a stall in effect
void RAM::stopStall() {
    setRAMDisabled();
//commitCFR1();
    totalCommit();
}