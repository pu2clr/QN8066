/**
This  sketch is an example of using this library with the DIY Kit 5W-7W FM board.

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

Author: Ricardo Lima Caratti (PU2CLR) - 2024/06/14
*/

#include <QN8066.h>

#define PWM_PIN   9      // Arduino PIN used to control the output power of the transmitter via PWM.
#define FREQUENCY 1069   // 106.9 MHz 

QN8066 tx;
void setup() {
  Serial.begin(9600);
  while (!Serial) ;
  pinMode(PWM_PIN, OUTPUT); // Sets the Arduino PIN to operate with with PWM
  if (!tx.detectDevice()) {
    Serial.println("\nDevice QN8066 not detected");
    while (1);
  }
  delay(500); // Wait a bit while the system stabilizes.
  tx.setup();
  Serial.print("\nStarting...");
  tx.setTX(FREQUENCY);    // Chenge the FREQUENCY constant if you want other value
  Serial.print("\nBroadcasting...");
  analogWrite(9, 50);  // Power controller via PWM - It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void loop() {
}
