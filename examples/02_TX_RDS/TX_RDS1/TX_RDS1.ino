/*

UNDER CONSTRUCTION...

Checking QN8066 RDS feature. 
This  sketch is an example of using the QN8066 Arduino library with the DIY Kit 5W-7W  FM 
board. 

The purpose of this application is to guide developers in using the  functions implemented 
in this library to develop their own FM transmission station  using  the "KIT DIY 5W-7W FM 
board based on  QN8066". It  is important  to  emphasize that users should be aware of and 
comply with the  applicable  laws and  regulations  in  their location  when using this FM 
transmitter. The  following  table  illustrates  the  connections   between  the  KIT  and 
the Arduino Uno, Nano, or Pro Mini boards.

| Anduino Nano or Uno pin | Kit 5W-7W FM  |
| ----------------------- | ------------- | 
|          GND            |     GND       | 
|           D9            |     PWM       | 
|           A4            |     SDA       | 
|           A5            |     SCL       | 

Attention: Use the "Serial Monitor" function in the Arduino IDE to monitor the system 
           initialization process.

IMPORTANT: 

Regarding the "KIT DIY 5~7W FM TRANSMITTER,"  during  experiments, it was  observed that the 
PWM signal disrupts the I2C communication significantly. This is likely due to the proximity 
of the PWM conductor to the bus. To  send commands to  the QN8066, the  PWM is  disabled and 
re-enabled after the  command is  sent; this was  the method  found to  make the KIT receive 
commands. However,  with RDS  operation, there is  a  constant need  to send new information, 
requiring frequent stops in transmission. At the time of writing this text, some changes are 
being planned:  1) introducing  a resistor  (possibly 1K or more)  in  series  with  the PWM 
conductor; 2) connecting the PWM directly to the power controller input, distancing  it from 
the I2C bus, and if possible, removing the PCB trace that is near the bus.

Author: Ricardo Lima Caratti (PU2CLR) - 2024/06/17
*/

#include <QN8066.h>

#define PWM_PIN 9       // Arduino PIN used to control the output power of the transmitter via PWM.
#define FREQUENCY 1069  // 106.9 MHz - This library does not use floating-point data. 

#define RDS_REFRESH_TIME 30000    // Recomended one minute - 600000

QN8066 tx;

char str[80];

// Station Name (PS) messages
char *rdsPSmsg[] = { (char *)"PU2CLR  ",
                     (char *)"QN8066  ",
                     (char *)"ARDUINO ",
                     (char *)"LIBRARY ",
                     (char *)"FM TX   " };

// Radio Text (RT) messages
char *rdsRTmsg[] = { (char *)"PU2CLR QN8066 ARDUINO LIBRARY   ",
                     (char *)"FM TRANSMITTER WITH RDS SERVICE ",
                     (char *)"https://github.com/pu2clr/QN8066",
                     (char *)"BE MEMBER  FACEBOOK GROUP QN8066",
                     (char *)"QN8066 HOMEBREW FM TRANSMITTER " };

uint8_t idxRdsMsg = 0;
const uint8_t lastRdsMsg = (sizeof(rdsPSmsg) / sizeof(rdsPSmsg[0])) - 1;
long rdsTime = millis();

uint8_t pty = 0;

void setup() {

  pinMode(PWM_PIN, OUTPUT);  // Sets the Arduino PIN to operate with with PWM

  Serial.begin(9600);


  delay(1000);  // Wait a bit while the system stabilizes.

  if (tx.detectDevice()) {
    Serial.println("\nQN8066 detected");
  } else {
    Serial.println("\nQN8066 not detected");
    while (1)
      ;
  }

  // Sets some internal parameters
  tx.setup(1000 /* Crystal Divider */,
           false /* Mono = False => Stereo */,
           true /* RDS ON */,
           1 /*PreEmphasis = 75*/);

  Serial.print("\nStarting the system.");
  delay(500);

  tx.setTX(FREQUENCY);  // Chenge the FREQUENCY constant if you want other value

  // tx.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  // tx.rdsTxEnable(true);
  // tx.rdsSetTxLineIn(true);
  // tx.updateTxSetup(); // Not working so far

  tx.rdsSetPI(1069);
  
  delay(500);
  sendRDS();

  sprintf(str, "\n\nBroadcasting with RDS...");

  analogWrite(9, 50);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

/**
1: Stereo = On
2: Pre-emphasis = 50 us
3: RDS = Enable
4: Impedance = 20K
5: Sft Clip.. = Disable
6: Thres = 3dB
7: Gain Pilot = 10%
8: Dreq. Deriv. 74.52
9: Buffer gain = 3dB
10: RDS Freq. Dev. 4.55kHz
Only the PI Code left, which is set to "0000"
*/
void sendRDS() {

  analogWrite(9, 0);
  tx.rdsInitTx(0, 0, 0);
  tx.rdsSetPTY(pty++);  // Document.
  if (pty > 30) pty = 1;
  delay(100);
  if (++idxRdsMsg > lastRdsMsg) idxRdsMsg = 0;
  tx.rdsSendPS(rdsPSmsg[idxRdsMsg]);
  delay(100);
  tx.rdsSendRTMessage(rdsRTmsg[idxRdsMsg]);
  sprintf(str,"\nStation Name / PS: %s :", rdsPSmsg[idxRdsMsg]);
  Serial.print(str);
  sprintf(str,"\nRadio Text/ RT...: %s :", rdsRTmsg[idxRdsMsg]);
  Serial.print(str);
  delay(100);
  sprintf(str, "\nPTY.............:%2d - ERROR: %d",tx.rdsGetPTY(), tx.rdsGetError());
  Serial.print(str);
  analogWrite(9, 50);
}


void loop() {
  if ((millis() - rdsTime) > RDS_REFRESH_TIME) {
    sendRDS();
    rdsTime = millis();
  }
  delay(5);
}
