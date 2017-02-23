//
// Created by Nick on 01-Feb-17.
//


//Define Pinout Constants
//LCD Pins
#define LCD1 8  //LCD RESET
#define LCD2 13 //LCD RW
#define LCD3 9  //LCD Enable
#define LCD4 3  //D4
#define LCD5 5  //D5
#define LCD7 6  //D6
#define LCD8 7  //D7
//CS for Ethernet, SD Card
#define ETH_CS 10
#define SD_CS 4

//CS for DDS
#define DDS_CS 52   //System clock for SPI
//Used
#define MASTER_RESET 18 //Goes to DDS pin 14
#define IO_RESET 37         //Goes to DDS pin 71
#define IO_UPDATE 46    //Goes to DDS pin 59
#define DROVER 23         //Goes to DDS pin 61
#define DRCTL 25          //Goes to DDS pin 62
#define DRHOLD 27        //Goes to DDS pin 63
#define PLL_LOCK 17      //Goes to DDS Pin 19
#define SWEEP_RESET_TRIGGER 15
#define SWEEP_TRIGGER 14
#define FAUX_TRIGGER 16
#define P0 53        //Goes to DDS pin 54
#define P1 48          //Goes to DDS pin 53
#define P2 49          //Goes to DDS pin 52


//Unused
#define EXT_PWR_DWN 2 //Goes to DDS pin 18
#define F0 41         //Goes to DDS pin 49
#define F1 39          //Goes to DDS pin 50
#define TxENABLE 45   //Goes to DDS pin 41
#define OSK 24           //Goes to DDS pin 60
