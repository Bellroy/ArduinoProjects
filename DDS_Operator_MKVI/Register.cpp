#include "Arduino.h"
#include "SPI.h"
#include "Register.h"
#include "PinDefinitions.h"

//Constructor that takes the register's address and length
Register::Register(byte address, int dataLength) {
    _regLength = dataLength;
    _addr = address;
}

//Sends the write code for the register (it's address)
void Register::sendWriteAddr() {
//Serial.print("Sending Write Byte: ");
//Serial.println(_addr);
    SPI.transfer(DDS_CS, _addr, SPI_CONTINUE);
}

//Sends the read code for the regegister (0x80| it's address)
void Register::sendReadAddr() {
//Serial.print("Sending read byte: ");
//Serial.println(0x80|_addr);
    SPI.transfer(DDS_CS, 0x80 | _addr, SPI_CONTINUE);
}

//Write an array of bytes to the register(for use in sending custom length data)
void Register::writeBytes(byte *data, int length, int start) {
    for (int i = start; i < length - 1; i++) {
//Serial.print("Sending byte: ");
//Serial.println(data[i]);
        SPI.transfer(DDS_CS, data[i], SPI_CONTINUE);
    }
    SPI.transfer(DDS_CS, data[length - 1]);
}

//Write an array of bytes backward to the register (for use in sending custom length data)
//Used because the Arduino stores in opposite Endianess of the AD9910 so
//indexed structs are "backwards"
void Register::writeBytesREndian(byte *data, int length, int start) {
    for (int i = start; i > start - (length - 1); i--) {
//Serial.print("Sending byte: ");
//Serial.println(data[i]);
        SPI.transfer(DDS_CS, data[i], SPI_CONTINUE);
    }
    SPI.transfer(DDS_CS, data[start - (length - 1)]);
}

//Write the register's length of bytes to the Register from the given array
void Register::writeReg(byte *data) {
    sendWriteAddr();
    writeBytes(data, _regLength);
}

//Print to serial the data contained in the register
void Register::readout() {
    read();
    for (int i = 0; i < _regLength; i++) {
        Serial.println(_data[i]);
    }
}

//Read into private data storage, the register's values
void Register::read() {
    sendReadAddr();
    for (int i = 0; i < _regLength - 1; i++) {
        _data[i] = SPI.transfer(DDS_CS, 0, SPI_CONTINUE);
    }
    _data[_regLength - 1] = SPI.transfer(DDS_CS, 0);
}