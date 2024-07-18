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
#define FREQUENCY 1067   // 106.7 MHz - This library does not use floating-point data. 
                         // This approach helps to save microcontroller memory. 
                         // Therefore, to represent a frequency in the commercial FM band, 
                         // multiply the desired frequency by 10. In this case 106.7MHz is 1067.

QN8066 tx;

char str[80];

void setup() {

  uint8_t deviceList[5], deviceCount = 0;

  Serial.begin(9600);
  while (!Serial) ;

  pinMode(PWM_PIN, OUTPUT); // Sets the Arduino PIN to operate with with PWM

  delay(1000); // Wait a bit while the system stabilizes.

  if (tx.detectDevice()) {
    Serial.println("\nDevice QN8066 detected");
  } else {
    Serial.println("\nDevice QN8066 not detected");
    while (1);
  }

  deviceCount = tx.scanI2CBus(deviceList);
  if (deviceCount > 0) {
    for (uint8_t i = 0; i < deviceCount; i++) {
      sprintf(str, "\nDevice found  at: %x in HEX - %d in DEC", deviceList[i], deviceList[i]);
      Serial.print(str);
    }
  }

  /* 
  tx.setup(1000,   // Divider based on frequency select of reference clock source. Default 1000 (see Datasheet pag. 18 and 23). 
           false,  // Mono False => TX will start in stereo mode
           false); // TX will start in RDS 
  */

  tx.setup(1000,   // Divider based on frequency select of reference clock source. Default 1000 (see Datasheet pag. 18 and 23). 
           false,  // Mono False => TX will start in stereo mode
           false,  // TX will start in RDS OFF
               1,  // PreEmphasis = 75
               0,  // type of the reference clock source (0 = Inject sine-wave clock; 1 = Inject digital clock). 
               1,  // Image Rejection (0=LO<RF, image is in lower side; 1=LO>RF, image is in upper side). 
               0,  // TX soft clip threshold. Default 0. See Datasheet page 34. 
               3,  // Selection of 1 minute time for PA off when no audio (3 = Infinity (never); 2=59s; 1=58s; 0=57s). 
               9); // Gain of TX pilot to adjust pilot frequency deviation. See Datasheet page 34.

  Serial.print("\nStarting the system.");
  delay(500);
  tx.setTX(FREQUENCY);    // Chenge the FREQUENCY constant if you want other value


  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);

  analogWrite(9, 50);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void loop() {
    sprintf(str,"\nTop FSM state code: %d", tx.getStatus1().arg.FSM);
    Serial.print(str);
    delay(15000);
}
