#include "Setup.h"
#include "PinDefinitions.h"
#include "Definitions.h"
#include "LCD_handler.h"

//Function that sets up the PLL clock settings using a reference
// a pll object, the input clock (MHz) and the desired output clock (MHz)
boolean setupPLL(PLL &myPll, int inMHz, int outMHz) {
//reset SPI communications to ensure accurate communication
    resetSPI();
//Call quickSetup function of PLL class that peforms the real action
    myPll.quickSetup(inMHz, outMHz);
//Update the values so they take effect
    update();
    Serial.print(F("PLL..."));
//Clear communication again (purely precautionary)
    resetSPI();
//Check if PLL has locked
    if (myPll.isLocked()) {
//If Locked, attach interrupt pausing all action if lock is lost
        Serial.println(F("Locked!"));
        delay(1);
        attachInterrupt(PLL_LOCK, PLL_Lost_Lock, FALLING);
//Confirm successful setup of PLL
        return true;
    }
    else {
//If not locked, return false
        Serial.println(F("Failed - Check Clock/Clock Settings"));
        return false;
    }
}

//Function to be performed if PLL lock is lost after setup
//Blocks until lock is reaquired
void PLL_Lost_Lock() {
    Serial.print("PLL Lock Lost...");
    while (!digitalRead(PLL_LOCK)) {
        myLCDprint("PLL LOCK", "LOST!");
    }
    Serial.println("Reaquired");
}

//Function that sets up communications between the
//Arduino and the AD9910
boolean setupCommunications() {
//Start SPI and set it to desired protocol
    Serial.println(F("Starting SPI"));
    SPI.begin(DDS_CS); //Start SPI on chip select pin for AD9910
    SPI.setDataMode(DDS_CS, SPI_MODE0); //Set to Mode0 (look it up on wikipeida)
    SPI.setBitOrder(DDS_CS, MSBFIRST); //Set communications to MSB first
    SPI.setClockDivider(DDS_CS, 8); //Set clock divider up at 8 [84Mhz/8 ~ 10MHz ~ 10Megabits/s]
//Reset the DDS to default settings so it is in a known state
    Serial.print(F("Resetting DDS to default settings..."));
    DDSdefaultReset();
    Serial.println(F("Done"));
//Establish Communications with AD9910
    Serial.print(F("Communications... "));
    set4Wire(); //Function to put AD9910 in wire SPI mode
    resetSPI(); //Reset physical communications (precautionary)
//Check to ensure AD9910 has been set correctly
    CFR1.read(); //Read values from CFR1
    if (CFR1._data[0] == 0 && CFR1._data[1] == 0 &&
        CFR1._data[2] == 0 && CFR1._data[3] == 0x02) {
//If values are correct, return true
        Serial.println(F("Success!"));
        return true;
    }
    else {
//Otherwise return false
        Serial.println(F("Failed!"));
        SPI.end();
        return false;
    }
}

//Function to set AD9910 into 4-wire mode after defaults have been reset (defaut is 3-wire)
void set4Wire() {
//CFR1 values needed for 4 wire mode
    byte set4Wire[4] = {0x00, 0x00, 0x00, 0x02};
    resetSPI(); //Reset physical communications (precautionary)
    CFR1.writeReg(set4Wire); //Write values to buffer
    update(); //Update buffer values into register
}

//Physical Communications reset function
//clears buffers of AD9910 SPI reciever [NOT pre-register buffer values]
void resetSPI() {
    digitalWrite(IO_RESET, HIGH);
    delay(1);
    digitalWrite(IO_RESET, LOW);
    delay(1);
}

//Reset Function
//Resets DDS to DDS default settings
void DDSdefaultReset() {
    digitalWrite(MASTER_RESET, HIGH);
    delay(1);
    digitalWrite(MASTER_RESET, LOW);
    delay(1);
}

//Set pin modes to correct values
//PINS ON ARDUINO DO NOT WORK IF THEY HAVE NOT BEEN SET
void setPinModes() {
    Serial.print(F("Setting pin modes... "));
//Lock Profile into Profile 0
    pinMode(P0, OUTPUT);
    digitalWrite(P0, LOW);
    pinMode(P1, OUTPUT);
    digitalWrite(P1, LOW);
    pinMode(P2, OUTPUT);
    digitalWrite(P2, LOW);
//-----------------------------
    pinMode(SWEEP_RESET_TRIGGER, INPUT);
    pinMode(SWEEP_TRIGGER, INPUT);
    pinMode(FAUX_TRIGGER, OUTPUT);
    digitalWrite(FAUX_TRIGGER, LOW);
    pinMode(MASTER_RESET, OUTPUT);
    digitalWrite(MASTER_RESET, LOW);
    pinMode(26, OUTPUT);
    digitalWrite(26, LOW);
    pinMode(LCD4, OUTPUT);
    pinMode(IO_UPDATE, OUTPUT);
    digitalWrite(IO_UPDATE, LOW);
    pinMode(DROVER, INPUT);
    pinMode(DRCTL, OUTPUT);
    digitalWrite(DRCTL, LOW);
    pinMode(DRHOLD, OUTPUT);
    digitalWrite(DRHOLD, LOW);
    pinMode(PLL_LOCK, INPUT);
    pinMode(IO_RESET, OUTPUT);
    digitalWrite(IO_RESET, LOW);
    delay(1);
    Serial.println("Done");
    delay(1);
}

//fullSetup function runs all setup functions in the correct order to set up AD9910
void fullSetup() {
    Serial.println(F("Setting up device for DDS control"));
    setPinModes();
    int count = 0;
    while (count < 3 && !setupCommunications()) {
        count++;
    }
    if (count == 3) {
        Serial.println(F("Connections Failed"));
        while (true) { }
    }
    resetSPI();
    PLL pll = PLL();
    if (!setupPLL(pll, MHZinCLK, MHZoutCLK)) {
        Serial.println(F("PLL Lock Failed!"));
        while (true) { }
    }
    Serial.print(F("Creating Profile Controller..."));
    profile0 = Profile(SINGLE_TONE_0);
    Serial.println(F("Done"));
    Serial.print(F("Creating DRG..."));
    myDRG = DRG(5);
    Serial.println(F("Done"));
    Serial.print(F("Creating RAM controller..."));
    myRAM = RAM(CFR1, RAM_PROFILE_0, RAM_WRITE);
    Serial.println("Done");
    Serial.print(F("Creating Sweeper..."));
    sweeper = Ramper(myDRG, myRAM);
    Serial.println(F("Done"));
    Serial.println(F("Setup Compete"));
}