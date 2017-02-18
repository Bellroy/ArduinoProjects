#include "Definitions.h"
#include "PinDefinitions.h"

boolean fastUpdate = false;
boolean DHCP_ENABLE = false;
int MHZinCLK = 10;
int MHZoutCLK = 1000;
//Pre-defined multi-purpose buffers
byte Buffer[500];
byte smallBuffer[10];
Register CFR1(0x00, 4);
Register CFR2(0x01, 4);
Register CFR3(0x02, 4);
Register AUX_DAC_CONTROL(0x03, 4);
Register IO_UPDATE_RATE(0x04, 4);
Register FTW(0x07, 4);
Register POW(0x08, 2);
Register ASF(0x09, 4);
Register MULTICHIP_SYNC(0x0A, 4);
Register DIGITAL_RAMP_LIMIT(0x0B, 8);
Register DIGITAL_RAMP_STEP_SIZE(0x0C, 8);
Register DIGITAL_RAMP_RATE(0x0D, 4);
Register SINGLE_TONE_0(0x0E, 8);
Register RAM_PROFILE_0(0x0E, 8);
/*Register SINGLE_TONE_1(0x0F,8);
Register RAM_PROFILE_1(0x0F,8);
Register SINGLE_TONE_2(0x10,8);
Register RAM_PROFILE_2(0x10,8);
Register SINGLE_TONE_3(0x11,8);
Register RAM_PROFILE_3(0x11,8);
Register SINGLE_TONE_4(0x12,8);
Register RAM_PROFILE_4(0x12,8);
Register SINGLE_TONE_5(0x13,8);
Register RAM_PROFILE_5(0x13,8);
Register SINGLE_TONE_6(0x14,8);
Register RAM_PROFILE_6(0x14,8);
Register SINGLE_TONE_7(0x15,8);
Register RAM_PROFILE_7(0x15,8);*/
Register RAM_WRITE(0x16, 4);

void update() {
//Special update used to maximize speed of update
    //digitalWriteDirect(IO_UPDATE, HIGH);
    digitalWrite(IO_UPDATE, HIGH);
    if (!fastUpdate) {
        delayMicroseconds(1);
    }
    //digitalWriteDirect(IO_UPDATE, LOW);
    digitalWrite(IO_UPDATE, LOW);
}