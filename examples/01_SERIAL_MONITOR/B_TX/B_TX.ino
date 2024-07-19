/**
This  sketch is an example of using this library with the DIY Kit 5W-7W FM board.
It was modified and improved by Mr. Grazianny Carvalho Tavares, PU7MGR.

The purpose of this application is to guide developers in using the  functions implemented 
in this library to develop their own FM transmission station  using  the "KIT DIY 5W-7W FM 
board based on  QN8066". It  is important  to  emphasize that users should be aware of and 
comply with the  applicable  laws and  regulations  in  their location  when using this FM 
transmitter. The  following  table  illustrates  the  connections  between  the  KIT  and 
the Arduino Uno, Nano, or Pro Mini boards.

| Anduino Nano or Uno pin | Kit 5W-7W FM  |
| ----------------------- | ------------- | 
|          GND            |     GND       | 
|           D9            |     PWM       | 
|           A4            |     SDA       | 
|           A5            |     SCL       | 

Attention: Use the "Serial Monitor" function in the Arduino IDE to monitor the system 
           initialization process.

Author: Ricardo Lima Caratti (PU2CLR) and 
Grazianny Carvalho Tavares (PU7MGR) 
2024-07-19
*/

#include <QN8066.h>

#define PWM_PIN   9      // Arduino PIN used to control the output power of the transmitter via PWM.
#define FREQUENCY 1069   // 106.7 MHz - This library does not use floating-point data. 
                         // This approach helps to save microcontroller memory. 
                         // Therefore, to represent a frequency in the commercial FM band, 
                         // multiply the desired frequency by 10. In this case 106.7MHz is 1067.

QN8066 tx;

char str[80];

void setup() {

  Serial.begin(9600);
  while (!Serial) ;

  pinMode(PWM_PIN, OUTPUT);   // Sets the Arduino PIN to operate with with PWM

  delay(1000);                // Wait a bit while the system stabilizes.

  if (tx.detectDevice()) {
    Serial.println("\nDevice QN8066 detected");
  } else {
    Serial.println("\nDevice QN8066 not detected");
    while (1);
  }

  tx.setup();      // Sets some internal parameters

  Serial.print("\nStarting the system.");
  delay(500);
  
  tx.setTX(FREQUENCY);          // Chenge the FREQUENCY constant if you want other value
  tx.setToggleTxPdClear();      // TX aud_pk clear signal. Audio peak value is max-hold and stored
  tx.setPAC(56);                // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56
  tx.setTxStereo(true);         // Tx Stereo ON (true)
  tx.setTxPreEmphasis(75);      // Pre-Emphasis 50us ou 75uS (default is 75uS)
  tx.setTxOffAfterOneMinuteNoAudio(false);  // Selection of 1 minute time for PA off when no audio
  tx.setTxFrequencyDerivation(120);     // Specify total TX frequency deviation. TX frequency deviation 
                                        // 0.69KHz*TX_FEDV 0-255 (default is 108)
  tx.setTxPilotGain(8);                 // Pilot Again % (9%)
  tx.setTxSoftCliptTreshold(0);         // TX soft clip threshold (12’d2051 (3db back off from 0.5v) 
  tx.setTxSoftClippingEnable(false);    // TX soft clipping enable(default is false)
  tx.setTxInputImpedance(2);            // 0 = 10K; 1 = 20K; 2=40K(default); 3 = 80K 
  tx.setTxDigitalGain(0);               // TX digital gain => 0 = 0dB (default is 0 dB)
  tx.setTxInputBufferGain(4);           // TX Input Buffer Gain.  12 6 0 -6 (011)

  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);

  analogWrite(9, 50);                   // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void loop() {
    sprintf(str,"\nTop FSM state code: %d", tx.getStatus1().arg.FSM);
    Serial.print(str);
    delay(15000);
}
