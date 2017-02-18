#ifndef Ethernet_Handler_h
#define Ethernet_Handler_h

#include <Ethernet.h>
#include "Ramp.h"

//Variables that need to accessed out of the context of Ethernet_Handler
extern EthernetClient client;
extern EthernetServer server;

//Functions pertaining to general Ethernet port use
void setup_Ethernet(boolean DHCP_ENABLE);

byte loadRamps(Ramp *bufferArray);

boolean sendOutRamps(int numOfRamps, Ramp *rampArray);

boolean sendAddressAcross();

void sendCommand(char c);

void sendCommandStatus(boolean v);

void sendSuccessfulHeader();

void endClientTransmission();

void sendErrorMessage(char *errorMessage);

void sendFailHeader();

unsigned int getJSONint(char token);

boolean navigateTo(char *token, int N, int count = 500);

char *readUpTo(char token);

boolean handleInformation();

#endif