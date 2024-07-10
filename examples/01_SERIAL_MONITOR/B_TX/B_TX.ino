#include <QN8066.h>

#define FREQ 106.1

QN8066  dv; 



void setup() {

    char str[80];

    Serial.begin(9600);
    while(!Serial);


    pinMode(9, OUTPUT);


    sprintf(str,"\n\nBroadcasting on 106.1 MHz frequency");
    Serial.print(str);

    dv.setFrequency(FREQ);

    analogWrite(9, 100);


}




void loop() {

 }

 