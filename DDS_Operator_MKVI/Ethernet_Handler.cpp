//
// Created by Nick on 01-Feb-17.
//

#include "Ethernet_Handler.h"


//Variables used to define Ethernet constants
EthernetClient client; // Client instance for connected client
EthernetServer server(80); //Server instance on port 80
byte mac[] = {0x90, 0xa2, 0xda, 0x0f, 0x19, 0x4c}; //MAC address
IPAddress ip(192, 168, 0, 178); // Static IP address
//small Char buffer
char dataBuffer[10];

//Sets up Ethernet after Arduino boot
void setup_Ethernet(boolean DHCP_ENABLE) {
//Use DHCP if DHCP_ENABLE is true (set by SD card: false by default)
    if (DHCP_ENABLE) {
        Serial.print(F("Leasing IP from DHCP..."));
        if (Ethernet.begin(mac)) {
            Serial.println(F("Success"));
        }
        else {
            Serial.println(F("Failed"));
        }
    }
//Otherwise ues the hardcoded static IP
    else {
        Serial.print(F("Setting up at static ip: "));
        Ethernet.begin(mac, ip);
        Serial.println(Ethernet.localIP());
    }
//Start the server (Listens for connections)
    server.begin();
}

//Write the currently saved ramps into the JSON response
boolean sendOutRamps(int numOfRamps, Ramp *rampArray) {
    client.print(",\"ramps\":[");
    for (int i = 0; i < numOfRamps; i++) {
        client.print("{");
        client.print("\"l\":");
        client.print(rampArray[i].lowWord);
        client.print(" , ");
        client.print("\"h\":");
        client.print(rampArray[i].highWord);
        client.print(" , ");
        client.print("\"dir\":");
        client.print(rampArray[i].direction);
        client.print("}");
        if (i != numOfRamps - 1) {
            client.print(",");
        }
    }
    client.print("]");
    return true;
}

//Load ramps passsed in the payload "PL="
byte loadRamps(Ramp *bufferArray) {
//Get the number of ramps to load (last part of the command)
    char temp[5];
    client.readBytesUntil(':', temp, 5);
    byte numOfRamps = atoi(temp);
//Inform if there are too many ramps for a sweep
    if (numOfRamps > 255) {
        Serial.print(numOfRamps);
        Serial.println(F(" is too many ramps for one sweep! (Max=255)"));
        sendErrorMessage("Too many Ramps; Only 255 Ramps supported to keep resources free");
        return false;
    }
//Create temporary Ramp to write values into
    Ramp tempRamp = Ramp();
//Navigate to payload
    navigateTo("PL=", 3);
    int tic = millis();//For performance monitoring
    for (int j = 0; j < numOfRamps; j++) {
//If indices don't match, cancel load
        if (j = !getJSONint('n') - 1) {
            Serial.println(F("Mismatch in ramp indexes: Load cancelled."));
            return false;
        }
        myLCDprint("Loading Ramp", j);
//Find values in JSON string and set them as the values for the temporary Ramp
        tempRamp.highWord = getJSONint('h');
        tempRamp.lowWord = getJSONint('l');
        tempRamp.timeStep = getJSONint('t');
        tempRamp.wordStep = getJSONint('w');
        tempRamp.noDwellHigh = getJSONint('H');
        tempRamp.noDwellLow = getJSONint('L');
        tempRamp.wordDest = getJSONint('D');
        tempRamp.direction = getJSONint('d');
//Copy the temporary ramp into the buffer array
        bufferArray[j] = tempRamp;
    }
    int toc = millis(); //For performance monitoring
    Serial.print("Time taken to load ramps to RAM: ");
    Serial.print(toc - tic);
    Serial.println(" milliseconds");
//Return the number of ramps loaded
    return numOfRamps;
}

//Writes the device's IP address into the JSON response
boolean sendAddressAcross() {
    client.print(",\"address\": \"");
    client.print(Ethernet.localIP());
    client.print("\"");
    return true;
}

//Writes the given character into the JSON response
void sendCommand(char c) {
    client.print(",\"C\": \"");
    client.print(c);
    client.print("\"");
}

//Writes the given command status result into the JSON response
void sendCommandStatus(boolean v) {
    client.print(",\"CS\":");
    client.print(v);
}

//Write headers for a successful communication with the DDS
void sendSuccessfulHeader() {
//HTTP OK Response
    client.println("HTTP/1.1 200 OK");
//Needed for cross domian JSON requests
    client.println("Access-Control-Allow-Origin: null");
//Informs the browser that the DDS will close the connection upon finishing writing information
    client.println("Connection: close");
//The data sent in the response will be JSON
    client.println("Content-Type: application/json");
//Blank line seperating JSON from data
    client.println();
//Begin the JSON response with a "transmission" : "Okay" field
    client.print("{\"transmission\":\"Okay\"");
}

//End the JSON object begun with sendSuccesfulHeader
void endClientTransmission() {
    client.println("}");
}

//Write an error message into the JSON response
void sendErrorMessage(char *errorMessage) {
    client.print("\"error\":\"");
    client.print(errorMessage);
    client.println("\"");
}

//Write a 404 -- Page Not Found error to the client
//Used when it can not find the webpage on the SD card
void sendFailHeader() {
    client.println("HTTP/1.0 404 Not Found");
}

//Function that finds a JSON key character in the Client stream
unsigned int getJSONint(char token) {
//create acutal key sequence string -- "X":
    char key[4] = {'"', token, '"', ':'};
//Pass key to navigateTo
    navigateTo(key, 4);
//read up to next key and convert value to integer
    unsigned int val = atoi(readUpTo(','));
//return value
    return val;
}

//Write DDS information into JSON response
boolean handleInformation() {
    client.print(F(",\"info\":{\"clock\":"));
//System clock given by PLL static variable
    client.print(PLL::fSysClock);
    client.print(F(",\"status\":\"Idle\"}"));
    return true;
}

//Function to read up to a given token in the client stream
//Returns the Data up to and including the token
char *readUpTo(char token) {
    memset(dataBuffer, ' ', 10);
    if (client.readBytesUntil(token, dataBuffer, 10) == 0) {
        Serial.println("Couldn't Find Token");
    }
    return dataBuffer;
}

//Experimental function to take a file name
//and write the file to the client
boolean writeFileToClient(char *file) {
    if (!SD.exists(file)) {
        return false;
    }
    myFile = SD.open(file);
    if (!myFile) {
        return false;
    }
    while (client.connected() && myFile.available()) {
        memset(buffer, 0, sizeof(buffer));
        myFile.read(buffer, sizeof(buffer));
        client.write(buffer, sizeof(buffer));
    }
    return true;
}

//Experimental function to take data from the client
//and write it to a given file name on the SD card
boolean writeClientStreamToFile(char *file) {
    if (SD.exists(file)) {
        return false;
    }
    myFile = SD.open(file, FILE_WRITE);
    if (!myFile) {
        return false;
    }
    while (client.available()) {
        memset(buffer, 0, sizeof(buffer));
        client.readBytes(buffer, sizeof(buffer));
        myFile.write(buffer, sizeof(buffer));
    }
}

//Function to naviagate up to and including a certain token
//Everything up to and the token is tossed
//Count defaults to 500 if not given and is the number of characters to look through before giving up

//returns true if found, false if not
boolean navigateTo(char *token, int N, int count) {
    int foundChars = N;
    while (foundChars > 0 && count > 0) {
        foundChars = N;
        for (int i = 0; i < N; i++) {
            char c = client.read();
            count--;
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
    if (count == 0) {
        return false;
    }
    return true;
}
