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

Author: Ricardo Lima Caratti (PU2CLR) - 2024/06/17
*/

#include <QN8066.h>

#define PWM_PIN   9      // Arduino PIN used to control the output power of the transmitter via PWM.
#define FREQUENCY 1067   // 106.7 MHz - This library does not use floating-point data. 
                         // This approach helps to save microcontroller memory. 
                         // Therefore, to represent a frequency in the commercial FM band, 
                         // multiply the desired frequency by 10. In this case 106.7MHz is 1067.

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
           true  /* RDS ON */); 

  Serial.print("\nStarting the system.");
  delay(500);
 
  tx.setTX(FREQUENCY);           // Chenge the FREQUENCY constant if you want other value

  tx.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  tx.setToggleTxPdClear();
  tx.setTxPilotGain(10);
  tx.setTxStereo(true);
  tx.setTxPreEmphasis(50);

  tx.setTxInputImpedance(1);  // 0=10; 1 = 20; 2=40 (default); 3=80. Kohms.
  tx.setAudioTxDiff(true);
  tx.setTxFrequencyDerivation(120); // Valid valued from 0 to 255

  // RDS setup
  tx.setTxRDS(true);
  // tx.setRDSLineIn(true);

  sprintf(str, "\n\nBroadcasting with RDS...");

  analogWrite(9, 50);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

bool toggle = true;  
void loop() {

  if (toggle)
    tx.writeTxRDSBuffer(strSDR1);
  else   
    tx.writeTxRDSBuffer(strSDR2);

  delay(100);
  // while (!tx.getTxRDSUpdated());  

  tx.setTxToggleRDSReady();
  toggle = !toggle;

  delay(30000);
}
