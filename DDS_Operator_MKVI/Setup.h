//
// Created by Nick on 01-Feb-17.
//

//Library defining functions needed to set up the DDS
#ifndef Setup_h
#define Setup_h

#include "PLL.h"
#include "Profile.h"
#include "DRG.h"
#include "RAM.h"
#include "Ramper.h"

//Globals
extern DRG myDRG;
extern RAM myRAM;
extern Ramper sweeper;
extern Profile profile0;

boolean setupPLL(PLL &myPll, int inMHz, int outMHz);

boolean setupCommunications();

void set4Wire();

void resetSPI();

void DDSdefaultReset();

void PLL_Lost_Lock();

void setPinModes();

void fullSetup();

#endif