#ifndef LCD_Handler_h
#define LCD_Handler_h

#include <LiquidCrystal.h>
#include <Ethernet.h>

//External LCD definition so other functions can access it if need be
extern LiquidCrystal lcd;

//Functions that print various data types to the
//2-line LCD
void myLCDprint(char *line1, int line2);

void myLCDprint(char *line1, IPAddress line2);

void myLCDprint(char *line1, char line2);

void myLCDprint(char *line1, unsigned int line2);

void myLCDprint(char *line1, char *line2);

//Function that sets up the LCD screen initially
void setupLCD();

#endif