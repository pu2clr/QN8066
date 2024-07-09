#include <QN8066.h>


QN8066  dv; 

void setup() {

    uint8_t deviceList[5], deviceCount = 0;

    Serial.begin(9600);
    while(!Serial);

    if ( dv.detectDevice() ) { 
        Serial.println("Device QN8066 detected");
    } else {
      Serial.println("Device QN8066 not detected");  
    }

    deviceCount = dv.scanI2CBus(deviceList);
    if ( deviceCount > 0  ) { 
        for ( uint8_t  i = 0; i < deviceCount; i++ ) { 
            Serial.print("\nDevice fount  at: ");
            Serial.print(deviceList[i]);
        }
    }

}

void loop() {

 }

 