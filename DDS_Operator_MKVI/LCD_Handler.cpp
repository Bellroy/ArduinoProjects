//
// Created by Nick on 01-Feb-17.
//

#include "LCD_Handler.h"

//Object that controls the LCD screen
//Needs these pins in this order
LiquidCrystal lcd(8, 13, 9, 3, 5, 6, 7);

//Initializes the LCD and prints message
void setupLCD() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("LCD Screen");
    lcd.setCursor(0, 1);
    lcd.print("Initialized");
}

//All LCD print functions do the same thing
//Print Argument 1 to the first line and Argument 2 to the second
void myLCDprint(char *line1, int line2) {
    myLCDprint(line1, (unsigned int) line2);
}

void myLCDprint(char *line1, IPAddress line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void myLCDprint(char *line1, char line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void myLCDprint(char *line1, unsigned int line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void myLCDprint(char *line1, char *line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}