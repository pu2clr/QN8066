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

#define PWM_PIN   9      // Arduino PIN used to control the output power of the transmitter via PWM.
#define FREQUENCY 1069   // 106.7 MHz - This library does not use floating-point data. 
                         // This approach helps to save microcontroller memory. 
                         // Therefore, to represent a frequency in the commercial FM band, 
                         // multiply the desired frequency by 10. In this case 106.9MHz is 1069.

QN8066 tx;

char str[80];
char strSDR1[] = "QN8066  ";
char strSDR2[] = "LIBRARY.";


void setup() {

  pinMode(PWM_PIN, OUTPUT); // Sets the Arduino PIN to operate with with PWM

  delay(1000); // Wait a bit while the system stabilizes.

  if (tx.detectDevice()) {
    Serial.println("\nQN8066 detected");
  } else {
    Serial.println("\nQN8066 not detected");
    while (1);
  }

  // Sets some internal parameters
  tx.setup(1000 /* Crystal Divider */,
           false /* Mono = False => Stereo */,
           true  /* RDS ON */,
           1 /*PreEmphasis = 75*/); 

  Serial.print("\nStarting the system.");
  delay(500);
 
  tx.setTX(FREQUENCY);           // Chenge the FREQUENCY constant if you want other value

  // tx.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  // tx.rdsTxEnable(true);
  // tx.rdsSetTxLineIn(true);
  // tx.updateTxSetup(); // Not working so far



  sprintf(str, "\n\nBroadcasting with RDS...");

  analogWrite(9, 0);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

bool toggle = true;  
void loop() {
  // tx.rdsSetTxToggle();
  // if (toggle)
    tx.rdsSendPS(strSDR1);
  // else   
   // tx.rdsSendPS(strSDR2);
  tx.rdsSetTxToggle();
  while (!tx.rdsGetTxUpdated());  
  toggle = !toggle;

  delay(10000);

}
