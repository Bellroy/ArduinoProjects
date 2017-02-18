//Library for handling SD functions
#ifndef SD_Handler_h
#define SD_Handler_h

#include <SD.h>

extern boolean SDisBegun;
extern File myFile;

void setupSD();

boolean navigateToSD(char *token, int N);

#endif