/**

This  sketch is an example of using this library with the DIY Kit 5W-7W FM  board

| Anduino Nano or Uno pin | Kit 5W-7W FM  |
| ----------------------- | ------------- | 
|          GND            |     GND       | 
|           D9            |     PWM       | 
|           A4            |     SDA       | 
|           A5            |     SCL       | 


Author: Ricardo Lima Caratti (PU2CLR) - 2024/06/14

*/

#include <QN8066.h>

#define FREQ 1067   // Frequency: 106.7 MHz

QN8066 tx;


char str[80];

void setup() {

  uint8_t deviceList[5], deviceCount = 0;

  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(9, OUTPUT);
  delay(2000);

  if (tx.detectDevice()) {
    Serial.println("\nDevice QN8066 detected");
  } else {
    Serial.println("\nDevice QN8066 not detected");
    while (1)
      ;
  }

  deviceCount = tx.scanI2CBus(deviceList);
  if (deviceCount > 0) {
    for (uint8_t i = 0; i < deviceCount; i++) {
      sprintf(str, "\nDevice found  at: %x in HEX - %d in DEC", deviceList[i], deviceList[i]);
      Serial.print(str);
    }
  }

  tx.setup();
  Serial.print("\nStarting the system.");
  delay(4000);
  tx.setTX(FREQ);
  // tx.setTxOffAfterOneMinuteNoAudio(false); // The trasmitter will never sleep.
  tx.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  tx.setToggleTxPdClear();

  // tx.setAudioAnalogGain(0); // Em binário é 010 => -30dB
  // tx.setAudioDigitalGain(0);
  // tx.setTxPilotGain(10);

  tx.setTxStereo(true);
  // tx.setTxPreEmphasis(75);

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
