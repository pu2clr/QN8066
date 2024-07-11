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

  // dv.setup();
  // showStatus();
  // showSystem();
  dv.setTX(FREQ);
  dv.setPAC(56);  // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
  sprintf(str, "\n\nBroadcasting...");
  Serial.print(str);
  TCCR1B = TCCR1B & B11111000 | B00000001;  // Set PWM frequency to about 31 kHz
  analogWrite(9, 220);
  // showStatus();
  showSystem();

}


void showStatus() {
  qn8066_status1 s1 = dv.getStatus1();
  qn8066_status2 s2 = dv.getStatus2();
  qn8066_status3 s3 = dv.getStatus3();

  Serial.print("\n******* STATUS 1 ***************\n");
  sprintf(str,"\nStereo receiving status...........: %d", s1.arg.ST_MO_RX);
  Serial.print(str);
  sprintf(str,"\nRX Status.........................: %d", s1.arg.RXSTATUS);
  Serial.print(str);
  sprintf(str,"\nRX AGC Settling status............: %d", s1.arg.RXAGCSET);
  Serial.print(str);
  sprintf(str,"\nRXCCA status flag.................: %d", s1.arg.rxcca_fail);
  Serial.print(str);
  sprintf(str,"\nFSM...............................: %d", s1.arg.FSM);
  Serial.print(str);


  Serial.print("\n******* STATUS 2 ***************\n");
  sprintf(str,"\nRDS RX............................: %d", s2.raw);
  Serial.print(str);

  Serial.print("\n******* STATUS 3 ***************\n");
  sprintf(str,"\nRXAGC Error Flag...................: %d", s3.arg.rxagcerr);
  Serial.print(str);
  sprintf(str,"\nRDS TX.............................: %d", s3.arg.RDS_TXUPD);
  Serial.print(str);
  sprintf(str,"\nAudio peak value at ADC input....: %d", s3.arg.aud_pk);
  Serial.print(str);
  sprintf(str,"\nLarge CAP short detection flag....: %d", s3.arg.aud_pk);
  Serial.print(str);

}


void showSystem() {
  
  qn8066_system1 s1 = dv.getSystem1();
  qn8066_system2 s2 = dv.getSystem2();


  Serial.print("\n******* SYSTEM 1 ***************\n");
  sprintf(str,"\nCH (internal CCA).............: %d", s1.arg.cca_ch_dis);
  Serial.print(str);
  sprintf(str,"\nCH (internal CCS).............: %d", s1.arg.ccs_ch_dis);
  Serial.print(str);
  sprintf(str,"\nChannel Scan mode enable......: %d", s1.arg.chsc);
  Serial.print(str);
  sprintf(str,"\nTransmission request..........: %d", s1.arg.txreq);
  Serial.print(str);
  sprintf(str,"\nReceiving reques..............: %d", s1.arg.rxreq);
  Serial.print(str);
  sprintf(str,"\nStandby mode..................: %d", s1.arg.stnby);
  Serial.print(str);
  sprintf(str,"\nSYSTEM 1 Register.............: %d", s1.raw);
  Serial.print(str);  


  Serial.print("\n******* SYSTEM 2 ***************\n");
  sprintf(str,"\nPre-emphasis..................: %d", s2.arg.tc);
  Serial.print(str);
  sprintf(str,"\nRDS transmitting ready........: %d", s2.arg.rdsrdy);
  Serial.print(str);
  sprintf(str,"\nTX audio mute enabel..........: %d", s2.arg.tx_mute);
  Serial.print(str);
  sprintf(str,"\nSYSTEM 2 Register.............: %d", s2.raw);
  Serial.print(str);    

}




void loop() {
}
