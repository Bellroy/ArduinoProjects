// MK VI
//ChangeLog
//Bump function added
//Comments added
//Several performance modifications
//---------------------------------------
//
//Needed for Network Communications
#include <LiquidCrystal.h>
#include <Ethernet.h>
#include "LCD_Handler.h"
#include <SPI.h>
#include "Ramp.h"
#include <SD.h>
#include "SD_Handler.h"
#include "Ethernet_Handler.h"
//Need for DDS
#include "PinDefinitions.h"
#include "Definitions.h"
#include <SPI.h>
#include "Register.h"
#include "PLL.h"
#include "DRG.h"
#include "Profile.h"
//#include "Ramp.h"
#include "Ramper.h"
#include "RAM.h"
#include "Setup.h"
//For Checking Memory Use
#include "MemoryFree.h"
//Globals for DDS
DRG myDRG;
Profile profile0;
RAM myRAM;
Ramper sweeper;
//Ramp Array for storing a multi-part sweep
Ramp rampArray[255];
byte rampArrayLen = 0;
/*
//Function that performs a software reset
void Software_Reset() {
  Serial.println("Going down for reset!");
  const int RSTC_KEY = 0xA5;
  RSTC->RSTC_CR = RSTC_CR_KEY(RSTC_KEY) | RSTC_CR_PROCRST | RSTC_CR_PERRST;
  while (true);
}
*/
void myTest() {
  Ramp myRamp = Ramp(50000, 500000, 10000, 0, 0);
  Ramp r1 = Ramp(5000000, 50000000, 10000, 0, 0);
  Ramp r2 = Ramp(2000000, 5000000, 8000, 0, 0);
  Ramp r3 = Ramp(1000000, 2000000, 8000, 0, 0);
  Ramp myArray[] = {r1, r2, r3};
  profile0.produceFreq(10 * 1000 * 1000);
  Serial.println(F("Beginning Chained Sweep Test"));
  Serial.println(F("Begin"));
  //sweeper.chainedSweeps(myArray, 3);
  Serial.println(F("Finished"));
}
//In Arduino, "setup" function runs first, followed by continous running of "loop" function
//Runs once, sets up the Arduino for operation
void setup() {
  //Start the serial port at 9600 baud
  Serial.begin(9600);
  Serial.println(F("ARDUINO BOOTED"));
  Serial.print("Amount of free RAM pre-setup: "); Serial.println(freeMemory());
  //From LCD_Handler; sets up LCD display
  setupLCD();
  //From SD_Handler; sets up SD card
  setupSD();
  //From Ethernet_Handler; sets up Ethernet Host
  //Passed DHCP_ENABLE which is True/False
  setup_Ethernet(DHCP_ENABLE);
  //From Setup; sets up DDS communications and settings
  fullSetup();
  Serial.print("Amount of free RAM post-setup: "); Serial.println(freeMemory());
}
boolean handleSweeps() {
  boolean funcStatus = false;
  switch (client.read()) {
    case 'r':
      rampArrayLen = loadRamps( rampArray);
    case 'l':
      funcStatus = sendOutRamps(rampArrayLen, rampArray);
      break;
    case 'f':
      break;
          break;
  }
  return funcStatus;
}
boolean handleConstFreq() {
  char c = client.read();
  if (c == 'f') {
    memset(smallBuffer, 0, 10);
    client.readBytesUntil(':', smallBuffer, 10);
    int freq = atoi((char*)smallBuffer);
    Serial.print("Producing Single-Mode Frequency: "); Serial.println(freq);
    profile0.produceFreq(freq);
    return true;
  }
  // else if(c=='w'){
  // }
  return false;
}
char mainCharHandle(char readValue) {
  int returnVal = 1;
  boolean commandStatus = false;
  switch (readValue) {
    case 'c':
      commandStatus = handleConstFreq();
      break;
    case 's':
      commandStatus = handleSweeps();
      break;
    case 'G':
      sweeper.chainedSweeps(rampArray, rampArrayLen);
      commandStatus = true;
      break;
    case 'i':
      commandStatus = handleInformation();
      break;
    case '!':
      client.println("Stop function");
      break;
    case 'r':
      returnVal = 'r';
      break;
    case 'p':
      commandStatus = sendAddressAcross();
      break;
    case 't':
      myTest();
      commandStatus = true;
      break;
    default:
      readValue = '%';
      //client.println("Command not recognized; Displaying help");
      //showHelp();
  }
  sendCommand(readValue);
  sendCommandStatus(commandStatus);
  return returnVal;
}
//Loop function runs continuously
void loop() {
  Serial.print("Amount of free RAM: "); Serial.println(freeMemory());
  //Check if a client is available
  client = server.available();
  //Print IP address to Serial and LCD
  Serial.print(F("Ethernet server established at: ")); Serial.println(Ethernet.localIP());
  myLCDprint("Waiting at", Ethernet.localIP());
  //Block until a client is connected
  while (!client) {
    client = server.available();
  }
  //myLCDprint("Client Connected!","");
  char result;
  if (client.connected()) {
    client.setTimeout(10); //Set timeout on client operations to 10 ms
    if (!navigateTo("Command=", 8 , 200)) { //nagateTo function from Ethernet_Handler
      //If there is no command in the incoming request, serve the webpage
      Serial.println(F("No Command found"));
      //Check that that the SD card was initialized correctly(SD card is in)
      if (SDisBegun) {
        //Open the compressed webpage: indexc.txt
        Serial.print("Opening File: INDEXC.TXT");
        myFile = SD.open("INDEXC.TXT");
        //If file is opened correctly (it exists)
        if (myFile) {
          //Headers for the webpage
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type:text/html");
          client.println("Content-Encoding: gzip");
          client.println();
          //Copy data from file into buffer, and then to write to the client
          while (myFile.available() && client.connected()) {
            memset(Buffer, 0, 500);
            myFile.read(Buffer, sizeof(Buffer));
            client.write(Buffer, sizeof(Buffer));
          }
          //Cleanup after serving page
          Serial.println(F("Finished Sending webpage"));
          myFile.close();
        }
        else {
          //If file could not be opened
          sendFailHeader();
          Serial.println(F("Couldn't find file"));
        }
      }
      else {
        //If SD card did was not initialized correctly
        sendFailHeader();
        Serial.println(F("SD card was not initialized correctly"));
      }
    }
    else {
      //If a command was found
      //Read (first) command byte into char variable
      char command = client.read();
      //Send out successful header indicating JSON response
      sendSuccessfulHeader();
      //Dispatch first command byte to function, store result
      result = mainCharHandle(command);
      //End the Response
      endClientTransmission();
    }
    //Close connection to client
    client.stop();
    //If reset condition has been made, perform software reset
    if (result == 'r') {
      //Software_Reset();
    }
  }
  client.stop();
}
