/**

This sketch works on Arduino Pro Mini, Nano, UNO, or any other board based on the ATMega328. 
Refer to the documentation if you are using a 5V microcontroller instead of a 3.3V one.

This sketch illustrates the clock configuration used on the XCLK pin of the QN8066. 
Depending on the active crystal or signal generator you are using, it is important 
to set the correct frequency and signal waveform type for the QN8066 to function 
properly.

| Anduino Nano or Uno pin | QN8066        |
| ----------------------- | ------------- | 
|          GND            |  GND          | 
|           A4            |  SDA (2)      | 
|           A5            |  SCL (1)      |
| Active Crystal          |               | 
|          source         |  XCLK (14)    |        


IMPORTANT: The QN8066 works with active crystals or signal generators. 
           Passive crystals will not work with the QN8066.

Attention: Use the "Serial Monitor" function in the Arduino IDE to monitor the system 
           initialization process.

Author: Ricardo Lima Caratti (PU2CLR) - 2024
*/

#include <QN8066.h>

#define FREQUENCY 1069   // 106.9 MHz - This library does not use floating-point data. 
                         // This approach helps to save microcontroller memory. 
                         // Therefore, to represent a frequency in the commercial FM band, 
                         // multiply the desired frequency by 10. In this case 109.7MHz is 1069.
QN8066 tx;

char str[80];

void setup() {

  Serial.begin(9600);
  while (!Serial) ;

  delay(100); // Wait a bit while the system stabilizes.

  if (!tx.detectDevice()) {
    Serial.println("\nDevice QN8066 not detected");
    while (1);
  }

  tx.begin();
  // Assuming you are using a 32.768 kHz active crystal, a digital signal, and LO<RF, 
  // the image is on the lower side.
  tx.setXtal(1,1,0); 
  // tx.setXtal(100,1,0); // If you are using 3,768 active crystal
  // tx.setXtal(1000,1,0); // If you are using 32,768,000 active crystal

  delay(100);
  tx.setTX(FREQUENCY);    // Chenge the FREQUENCY constant if you want other value

  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);
}

void loop() {
    sprintf(str,"\nFSM: %d\nAudio Peak: %d mV", tx.getFsmStateCode(), tx.getAudioPeakValue());
    Serial.print(str);
    tx.resetAudioPeak();
    delay(15000);
}
