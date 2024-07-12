#include <QN8066.h>

#define FREQ 106.7

QN8066 dv;


char str[80];

void setup() {

  uint8_t deviceList[5], deviceCount = 0;

  Serial.begin(9600);
  while (!Serial)
    ;


  pinMode(9, OUTPUT);


  if (dv.detectDevice()) {
    Serial.println("\nDevice QN8066 detected");
  } else {
    Serial.println("\nDevice QN8066 not detected");
    while (1)
      ;
  }

  deviceCount = dv.scanI2CBus(deviceList);
  if (deviceCount > 0) {
    for (uint8_t i = 0; i < deviceCount; i++) {
      sprintf(str, "\nDevice found  at: %x in HEX - %d in DEC", deviceList[i], deviceList[i]);
      Serial.print(str);
    }
  }

  dv.setup();
  delay(2000);
  dv.setTX(FREQ);
  dv.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.

  // dv.setAudioAnalogGain(0); // Em binário é 010 => -30dB
  // dv.setAudioDigitalGain(0);
  dv.setTxPilotGain(10);
  sprintf(str, "GPLT (setTxPilotGain): %x", dv.getRegister(QN_GPLT));
  Serial.print(str);

  dv.setTxStereo(false);
  dv.setTxPreEmphasis(75);
  sprintf(str, "SYSTEM2 (Stereo/Mono and Pre-emphasis): %x", dv.getRegister(QN_SYSTEM2));
  Serial.print(str);


  dv.setTxInputImpedance(0); // 0=10; 1 = 20; 2=40 (default); 3=80. Kohms.


  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);
  TCCR1B = TCCR1B & B11111000 | B00000001;  // Set PWM frequency to about 31 kHz
  analogWrite(9, 127); 

  sprintf(str, "\n\nSYSTEM1: %d \nCCA: %d \nXTAL_DIV0: %d\nXTAL_DIV1: %d", dv.getRegister(QN_SYSTEM1), dv.getRegister(QN_CCA), dv.getRegister(QN_XTAL_DIV0),  dv.getRegister(QN_XTAL_DIV1));
  Serial.print(str);

}


void loop() {
}
