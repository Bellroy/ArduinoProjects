//
// Created by Nick on 01-Feb-17.
//


#ifndef Definitions_h
#define Definitions_h

#include "Register.h"
#include "DRG.h"
#include "RAM.h"
#include "Ramper.h"
/*Definitions File
This defines functions and objects that will want to be shared across libraries
*/

//Compiler Macro that defines a direct port write. Is much faster than digitalWrite (for time sensitive uses)

static inline void digitalWriteDirect(int pin, boolean val) {


    if (val) g_APinDescription[pin].pPort->PIO_SODR = g_APinDescription[pin].ulPin;
    else g_APinDescription[pin].pPort->PIO_CODR = g_APinDescription[pin].ulPin;
}


//Compiler Macro that defines a direct port read. Is much faster than digitaRead (for time sensitive uses)

static inline int digitalReadDirect(int pin) {
    return !!(g_APinDescription[pin].pPort->PIO_PDSR & g_APinDescription[pin].ulPin);

}

//Variables that define the state of the system extern boolean fastUpdate;
extern boolean DHCP_ENABLE;
extern int MHZinCLK;
extern int MHZoutCLK;

//Pre-defined multi-purpose buffers extern byte Buffer[500];
extern byte smallBuffer[10];

//Register Definitions extern Register CFR1; extern Register CFR2; extern Register CFR3;
extern Register AUX_DAC_CONTROL;
extern Register IO_UPDATE_RATE;
extern Register FTW;
extern Register POW;
extern Register ASF;
extern Register MULTICHIP_SYNC;
extern Register DIGITAL_RAMP_LIMIT;
extern Register DIGITAL_RAMP_STEP_SIZE;
extern Register DIGITAL_RAMP_RATE;
extern Register SINGLE_TONE_0;
extern Register RAM_PROFILE_0;
/*extern Register SINGLE_TONE_1(0x0F,8); extern Register RAM_PROFILE_1(0x0F,8);
extern Register SINGLE_TONE_2(0x10,8);
extern Register RAM_PROFILE_2(0x10,8);
extern Register SINGLE_TONE_3(0x11,8);
extern Register RAM_PROFILE_3(0x11,8);
extern Register SINGLE_TONE_4(0x12,8);
extern Register RAM_PROFILE_4(0x12,8);
extern Register SINGLE_TONE_5(0x13,8);
extern Register RAM_PROFILE_5(0x13,8);
extern Register SINGLE_TONE_6(0x14,8);
extern Register RAM_PROFILE_6(0x14,8);
extern Register SINGLE_TONE_7(0x15,8);
extern Register RAM_PROFILE_7(0x15,8);*/
extern Register RAM_WRITE;

//Update Function used to write values in buffer of AD9910 to active registers
void update();

#endif
