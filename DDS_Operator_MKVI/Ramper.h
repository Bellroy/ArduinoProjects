//Class that controls the DRG and RAM using instances
//of the Ramp class in order to create sweeps and chained sweeps
#ifndef Ramper_h
#define Ramper_h

#include "Arduino.h"
#include "DRG.h"
#include "Ramp.h"
#include "RAM.h"

class Ramper {
public:
    Ramper();

    Ramper(DRG &drg, RAM &ramObject);

    void loadSingleSweep(Ramp ramp);

    void doSweep(Ramp ramp);

    void chainedSweeps(Ramp *rampArray, int length);

    void beginSweep(boolean dir);

// static void beginSweepUp();
// static void beginSweepDown();
    static void trigger();

    static void resetTrigger();

    void Bump();

    void setDrgLow();

private:
    void turnOnDRG();

    RAM *myRAM;
    DRG *drg;
};

#endif