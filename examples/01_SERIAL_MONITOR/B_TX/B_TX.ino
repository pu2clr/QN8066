#include <QN8066.h>

#define FREQ 106.7

QN8066  dv; 



void setup() {

    uint8_t deviceList[5], deviceCount = 0;
    char str[80];

    Serial.begin(9600);
    while(!Serial);


    pinMode(9, OUTPUT);


    if ( dv.detectDevice() ) { 
        Serial.println("\nDevice QN8066 detected");
    } else {
      Serial.println("\nDevice QN8066 not detected");  
      while(1);
    }

    deviceCount = dv.scanI2CBus(deviceList);
    if ( deviceCount > 0  ) { 
        for ( uint8_t  i = 0; i < deviceCount; i++ ) { 
            sprintf(str,"\nDevice found  at: %x in HEX - %d in DEC", deviceList[i], deviceList[i]);
            Serial.print(str);
        }
    }

    dv.setTX();
    dv.setPAC(55); // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
    sprintf(str,"\n\nBroadcasting on 106.7 MHz");
    Serial.print(str);
    dv.setFrequency(FREQ);
    TCCR1B = TCCR1B & B11111000 | B00000001;  // Set PWM frequency to about 31 kHz
    analogWrite(9, 100);


}




void loop() {

 }

 