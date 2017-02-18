#include "DRG.h"
#include "PinDefinitions.h"
#include "Definitions.h"

//Blank Constructor so memory can be set aside before DRG class is actually initialized
DRG::DRG() { }

//Different constructor called when it is actually time to set up DRG
//Blah parameter is to differentiate the constructor; never used
DRG::DRG(int blah) {
//Read registers needed for DRG
    CFR1.read();
    CFR2.read();
    DIGITAL_RAMP_RATE.read();
    DIGITAL_RAMP_STEP_SIZE.read();
    DIGITAL_RAMP_RATE.read();
//Read Register data values into DRG private register data reserves
    for (int i = 0; i < 4; i++) {
        _CFR1[i] = CFR1._data[i];
        _CFR2[i] = CFR2._data[i];
        _DRR[i] = DIGITAL_RAMP_RATE._data[i];
    }
    for (int i = 0; i < 8; i++) {
        _DRL[i] = DIGITAL_RAMP_LIMIT._data[i];
        _DRS[i] = DIGITAL_RAMP_STEP_SIZE._data[i];
    }
}

//Send Digital Ramp Direction Control pin Low (Downward sweep)
void DRG::slopeControlLow() {
    digitalWrite(DRCTL, LOW);
}

//Send Digital Ramp Direction Control pin High (Upward sweep)
void DRG::slopeControlHigh() {
    digitalWrite(DRCTL, HIGH);
}

//Pause current DRG sweep
void DRG::hold() {
    digitalWrite(DRHOLD, HIGH);
}

//Continue current DRG sweep
void DRG::release() {
    digitalWrite(DRHOLD, LOW);
}

//Return true if DRG has reached bottom or upper limit
boolean DRG::isAtLimit() {
    return digitalReadDirect(DROVER);
}

//Set Digital Ramp Destination Bit to value
void DRG::setDigitalRampDestination(byte value) {
    _CFR2[1] = (value << 4) | ((B11001111) & (_CFR2[1]));
}

//Set or unset Digital Ramp Enable bit
void DRG::setDigitalRampEnable(boolean value) {
    _CFR2[1] = (value << 3) | ((B11110111) & (_CFR2[1]));
}

//Commit changes to CFR2 Register (values stored in buffer, not active until an Update)
void DRG::commitCFR2() {
    CFR2.writeReg(_CFR2);
}

//Set Upper Limit of the sweep. Takes a tuning word
void DRG::setUpperLimit(unsigned int tuningWord) {
    _upperLimit.value = tuningWord;
//Serial.print("Upper Limit set to: ");
//Serial.println(_upperLimit.value);
}

//Set Lower Limit of the sweep. Takes a tuning word
void DRG::setLowerLimit(unsigned int tuningWord) {
    _lowerLimit.value = tuningWord;
//Serial.print("Lower Limit set to: ");
//Serial.println(_lowerLimit.value);
}

//Commit values of the Upper and Lower sweep limits to the buffer (not active until an Update)
void DRG::commitLimits() {
    DIGITAL_RAMP_LIMIT.sendWriteAddr();
    DIGITAL_RAMP_LIMIT.writeBytesREndian(_upperLimit.b, 4, 3);
    DIGITAL_RAMP_LIMIT.writeBytesREndian(_lowerLimit.b, 4, 3);
}

//Set or unset No-Dwell High Bit
void DRG::setNoDwellHigh(boolean value) {
    _CFR2[2] = (value << 2) | ((B11111011) & (_CFR2[2]));
}

//Set or unset No-Dwell Low Bit
void DRG::setNoDwellLow(boolean value) {
    _CFR2[2] = (value << 1) | ((B11111101) & (_CFR2[2]));
}

//Set negative slope to value
void DRG::setNegativeSlope(unsigned short value) {
    _negSlope.value = value;
}

//Set positive slope to value
void DRG::setPositiveSlope(unsigned short value) {
    _posSlope.value = value;
}

//Commit positive and negative slope valeus to buffer (Don't take effect until an Update)
void DRG::commitSlopes() {
    DIGITAL_RAMP_RATE.sendWriteAddr();
    DIGITAL_RAMP_RATE.writeBytesREndian(_negSlope.b, 2, 1);
    DIGITAL_RAMP_RATE.writeBytesREndian(_posSlope.b, 2, 1);
}

//Set the positive step rate to the given value
void DRG::setPositiveStepRate(unsigned int step) {
    _posStep.value = step;
}

//Set the negative step rate to the given value
void DRG::setNegativeStepRate(unsigned int step) {
    _negStep.value = step;
}

//Commit positive and negative step rates to buffer (Don't take effect until an Update)
void DRG::commitStepRates() {
    DIGITAL_RAMP_STEP_SIZE.sendWriteAddr();
    DIGITAL_RAMP_STEP_SIZE.writeBytesREndian(_negStep.b, 4, 3);
    DIGITAL_RAMP_STEP_SIZE.writeBytesREndian(_posStep.b, 4, 3);
}

//Commit all values related to DRG to buffer (Don't take effect until an Update)
void DRG::commit() {
    commitCFR2();
    commitLimits();
    commitSlopes();
    commitStepRates();
}