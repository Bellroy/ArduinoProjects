#include "Arduino.h"
#include "Ramper.h"
#include "PinDefinitions.h"
#include "Definitions.h"

//Blank constructor. Sets aside memory for object before it is initialized
Ramper::Ramper() { }

//Normal Constructor. Ramper only needs a reference to a DRG object instance and a RAM object instance
Ramper::Ramper(DRG &myDrg, RAM &ramObject) {
    drg = &myDrg;
    myRAM = &ramObject;
}

//Turns on the DRG as a frequency word source
void Ramper::turnOnDRG() {
    drg->setDigitalRampEnable(true);
    drg->commitCFR2();
    update();
}

//Performs a single sweep. Takes one sweep to perform
void Ramper::doSweep(Ramp ramp) {
    loadSingleSweep(ramp);
    turnOnDRG();
    beginSweep(ramp.direction);
}


//class variable informing whether a sweep has been loaded
boolean loaded = false;
//class variable informing if the device has been told to come out of sweep mode
boolean RESET_FLAG = false;
boolean bumped = false;


void setDrgLow() {
    detachInterrupt(DROVER);
    digitalWriteDirect(DRCTL, LOW);
    attachInterrupt(SWEEP_TRIGGER, Bump, RISING);
}

void Bump() {
    detachInterrupt(SWEEP_TRIGGER);
    bumped = true;
    digitalWriteDirect(DRCTL, HIGH);
    attachInterrupt(DROVER, setDrgLow, RISING);
}

//Chains sweeps in rampArray together to perform one constant sweep
//Length is the number of sweeps to go through
void Ramper::chainedSweeps(Ramp *rampArray, int length) {
    RESET_FLAG = false; //set reset flag to false
//attach sweepReset interrupt
    attachInterrupt(SWEEP_RESET_TRIGGER, Ramper::resetTrigger, RISING);
    loaded = false; //Set loaded to false as a ramp hasn't been loaded yet
//atttach sweep trigger interrupt
    attachInterrupt(SWEEP_TRIGGER, Ramper::trigger, RISING);
    turnOnDRG(); //Ensure that the DRG is in control of word generation for the AD9910
    myRAM->setSingleTone(); //Ensure that the Profile is in RAM mode in case single tone was used before
    //For each ramp in the array:
    for (int i = 0; i < length; i++) {
//Load sweep
        if (!loaded) {
            loadSingleSweep(rampArray[i]);
            loaded = true;
        }
//Standalone Trigger mechanism
//Waits until sweep is completed, triggers sweep pin
        while (!drg->isAtLimit()) { }
        digitalWrite(FAUX_TRIGGER, HIGH);
        while (loaded) {
            if (RESET_FLAG) {
                detachInterrupt(SWEEP_RESET_TRIGGER);
                detachInterrupt(SWEEP_TRIGGER);
                return;
            }
        };
        digitalWrite(FAUX_TRIGGER, LOW);
        int tic = micros();
        beginSweep(rampArray[i].direction);
        int toc = micros();
        Serial.print("Time taken to begin sweep:");
        Serial.println(toc - tic);
    }
//Sweep stored after the other sweeps is the "Bump" sweep
    loadSingleSweep(rampArray[length]);
    int tic = millis();
    bumped = false;
    int timeout = 60 * 1000; // 1 minute
    attachInterrupt(SWEEP_TRIGGER, Bump, RISING);
//Stay in bump mode until reset is triggered or timeout occurs
    while (!RESET_FLAG && millis() - tic < timeout) {
        if (bumped) {
            tic = millis();
            bumped = false;
        }
    }
    detachInterrupt(SWEEP_TRIGGER);
    detachInterrupt(SWEEP_RESET_TRIGGER);
}

//Loads a single sweep into the AD9910
void Ramper::loadSingleSweep(Ramp ramp) {
    int tic = micros(); //For perfomance monitoring
//Set DRG parameters to those given by the ramp
    drg->setNoDwellHigh(ramp.noDwellHigh);
    drg->setNoDwellLow(ramp.noDwellLow);
    drg->setDigitalRampDestination(ramp.wordDest);
    drg->setUpperLimit(ramp.highWord);
    drg->setLowerLimit(ramp.lowWord);
//enable DRG ramp
    drg->setDigitalRampEnable(true);
//If a positive sweep:
    if (ramp.direction) {
        drg->setPositiveSlope(ramp.timeStep);
        drg->setNegativeSlope(1); //Fastest Step
        drg->setPositiveStepRate(ramp.wordStep);
        drg->setNegativeStepRate(0xFFFF); //Largest Step
        drg->slopeControlLow();
    }
//If a negative sweep
    else {
        drg->setPositiveSlope(1);//Fastest Step
        drg->setNegativeSlope(ramp.timeStep);
        drg->setPositiveStepRate(0xFFFF); //Largest Step
        drg->setNegativeStepRate(ramp.wordStep);
        myRAM->setupStall(ramp.highWord);
    }
//Commit values to BUFFER,
//new ramp values will not take effect until an update
    drg->commit();
    int toc = micros(); //for performance monitoring
    Serial.print("Time taken to load ramp: ");
    Serial.println(toc - tic);
}

//Trigger function -- indicates new ramp needs to be loaded
void Ramper::trigger() {
    loaded = false;
}

void Ramper::resetTrigger() {
    RESET_FLAG = true;
}



//Function to begin the currently loaded (in buffer) sweep
void Ramper::beginSweep(boolean dir) {
    if (dir) {
        update();
        digitalWriteDirect(DRCTL, HIGH);
//drg->slopeControlHigh();
    }
    else {
//Then we want to stall at ramp.highWord
        update();
//Then we want to sweep up ramp until high
        digitalWriteDirect(DRCTL, HIGH);
//drg->slopeControlHigh();
        while (!digitalReadDirect(DROVER)) { }
//Remove RAM control
        myRAM->stopStall();
//Begin sweep down
        digitalWriteDirect(DRCTL, LOW);
//drg->slopeControlLow();
    }
}