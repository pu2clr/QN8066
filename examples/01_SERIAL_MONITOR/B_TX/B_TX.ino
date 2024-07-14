#include <QN8066.h>

#define FREQ 106.8

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
  // dv.setTxPilotGain(10);
  // sprintf(str, "GPLT (setTxPilotGain): %x", dv.getRegister(QN_GPLT));
  // Serial.print(str);

  dv.setTxStereo(true);
  dv.setTxPreEmphasis(75);
  // sprintf(str, "SYSTEM2 (Stereo/Mono and Pre-emphasis): %x", dv.getRegister(QN_SYSTEM2));
  // Serial.print(str);


  dv.setTxInputImpedance(0);  // 0=10; 1 = 20; 2=40 (default); 3=80. Kohms.
  dv.setTxInputBufferGain(5); // With input inpedance  0 (10K), and input buffer 5, the gain shoud be 18dB
  dv.setTxSoftClipping(true);
  dv.setTxDigitalGain(2); // TX digital gain => 2 = 2dB  (default is 0 dB)
  dv.setTxFrequencyDerivation(200); // Valid valued from 0 to 255

  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);

  analogWrite(9, 80); 

  sprintf(str, "\n\nSYSTEM1: %X (Hex)\nSYSTEM2: %X (Hex)", dv.getRegister(QN_SYSTEM1), dv.getRegister(QN_SYSTEM2));
  Serial.print(str);

}


void loop() {
}
