
#include <QN8066.h>

QN8066 dv;

void setup() {

  Serial.begin(9600);
  while (!Serial); 

  pinMode(9, OUTPUT);  // Pin use to PWM power control
 
  dv.setup();
  Serial.print("\nStarting the system...");
  delay(1000);      
  dv.setTX(1069); // Set the transmitter to 106.7 MHz 
  dv.setTxStereo(true);

  // Now you can start PWM
  analogWrite(9, 50);  // It is about 1/5 of the max power (5~7W). It is between 1 and 1,4 W
  Serial.print("\nBroadcasting...");

  // Turning the Stereo OFF
  analogWrite(9, 0); // Disable PWM
  dv.setTxStereo(false);
  analogWrite(9, 50); // Enable PWM


}

void loop() {
  delay(5);
}