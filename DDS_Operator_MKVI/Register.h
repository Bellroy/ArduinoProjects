//Register class -- Backbone of AD9910 communication and programming
#ifndef Register_h
#define Register_h

#include "Arduino.h"
#include "SPI.h"

class Register {
public:
    Register(byte address, int dataLength);

    void readout();

    void writeReg(byte *data);

    void read();

    void sendReadByte();

    void sendWriteByte();

    void sendReadAddr();

    void sendWriteAddr();

    void writeBytesREndian(byte *data, int length, int start);

    void writeBytes(byte *data, int length, int start = 0);

    byte _data[8];
    byte _addr;
    byte _regLength;
private:
};

#endif