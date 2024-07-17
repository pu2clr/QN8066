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

  tx.setup(); // Sets some internal parameters

  Serial.print("\nStarting the system.");
  delay(500);
  tx.setTX(FREQUENCY);    // Chenge the FREQUENCY constant if you want other value
  // tx.setTxOffAfterOneMinuteNoAudio(false); // The trasmitter will never sleep.
  tx.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  tx.setToggleTxPdClear();

  tx.setTxPilotGain(10);

  tx.setTxStereo(true);
  tx.setTxPreEmphasis(75);

  tx.setTxInputImpedance(0);  // 0=10; 1 = 20; 2=40 (default); 3=80. Kohms.
  tx.setTxInputBufferGain(5); // With input inpedance  0 (10K), and input buffer 5, the gain shoud be 18dB
  tx.setTxSoftClippingEnable(false);
  tx.setTxSoftCliptTreshold(2);
  tx.setAudioTxDiff(true);
  tx.setTxDigitalGain(2); // TX digital gain => 2 = 2dB  (default is 0 dB)

  tx.setTxFrequencyDerivation(200); // Valid valued from 0 to 255

  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);

  analogWrite(9, 50);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void loop() {
    sprintf(str,"\nTop FSM state code: %d", tx.getStatus1().arg.FSM);
    Serial.print(str);
    delay(15000);
}
