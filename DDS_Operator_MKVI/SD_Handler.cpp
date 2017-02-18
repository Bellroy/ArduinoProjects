//
// Created by Nick on 01-Feb-17.
//

#include "SD_Handler.h"


//Boolean to check if an SD card has been inserted
boolean SDisBegun = false;
//File instance put aside to be used for all File I/O operations
File myFile;
//general use buffer for SD functions
char charBuffer[15];

//Handles initialization of SD card [Run during setup]
void setupSD() {
    Serial.print("SD Card...");
//If SD card cannot be begun (things are not wired correctly, SD card not inserted)
    if (!SD.begin(4)) {
        Serial.println("Not Inserted: Using default setup values");
    }
//Otherwise
    else {
        SDisBegun = true;
        Serial.println(F("Found"));
//Open config file
        myFile = SD.open("Config.txt");
//If file exists (opens)
        if (myFile) {
            Serial.println(F("Using setup values in Config.txt:"));
//Navigate to CLK_IN parameter and use it
            if (navigateToSD("CLK_IN(MHZ):", 12)) {
                memset(charBuffer, ' ', 15);
                myFile.read(charBuffer, sizeof(charBuffer));
                int val = atoi(charBuffer);
                Serial.print(" Clock In: ");
                Serial.println(val);
                MHZinCLK = val;
            }
//Navigate to CLK_OUT parameter and use it
            if (navigateToSD("CLK_OUT(MHZ):", 13)) {
                memset(charBuffer, ' ', 15);
                myFile.read(charBuffer, sizeof(charBuffer));
                int val = atoi(charBuffer);
                Serial.print(" Clock Out: ");
                Serial.println(val);
                MHZoutCLK = val;
            }
//Navigate to DHCP parameter and use it
            if (navigateToSD("DHCP:", 5)) {
                if (myFile.read() == 'E') {
                    Serial.print(" DHCP: ");
                    Serial.println("Enabled");
                    DHCP_ENABLE = true;
                }
                else {
                    Serial.print(" DHCP: ");
                    Serial.println("Disabled");
                    DHCP_ENABLE = false;
                }
            }
            myFile.close();
            Serial.println(F("Setup Values Input"));
        }
//If file can't be opened, print error
        else {
            Serial.println(F("Couldn't open file for reading"));
        }
    }
}

//Naviagte to function for SD card
boolean navigateToSD(char *token, int N) {
    myFile.seek(0);
    int foundChars = N;
    while (foundChars > 0 && myFile.available()) {
        foundChars = N;
        for (int i = 0; i < N; i++) {
            char c = myFile.read();
            if (c == token[i]) {
                foundChars--;
            }
            else if (c == token[0]) {
                foundChars = N - 1;
                i = 0;
            }
            else {
                break;
            }
        }
    }
    if (!myFile.available()) {
        return false;
    }
    return true;
}