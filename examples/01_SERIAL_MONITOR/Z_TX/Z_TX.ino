/**

| Anduino Nano or Uno pin | Kit 5W-7W FM  |
| ----------------------- | ------------- | 
|          GND            |     GND       | 
|           D9            |     PWM       | 
|           A4            |     SDA       | 
|           A5            |     SCL       | 

*/

#include <QN8066.h>


QN8066 tx;

void setup() {
  delay(1000);  
  Serial.begin(9600);
  while (!Serial);  
  Serial.print("\nStarting TX...");
  writeRegister(0x00, 0xE3);
  writeRegister(0x01, 0x00);
  writeRegister(0x02, 0x50);
  writeRegister(0x07, 0xE8);
  writeRegister(0x08, 0x0B);
  writeRegister(0x09, 0x5C);
  writeRegister(0x19, 0x22);
  writeRegister(0x1B, 0xF8);
  writeRegister(0x00, 0x0B);
  writeRegister(0x25, 0x7D);
  writeRegister(0x26, 0x3C);
  writeRegister(0x27, 0x39); 

  writeRegister(0x00, 0x0B);

  writeRegister(0x19, 0x23);
  writeRegister(0x1B, 0xA7); 

  writeRegister(0x49, 0xE8);
  writeRegister(0x1B, 0xA6);

  writeRegister(0x49, 0xDF);

  writeRegister(0x6E, 0xFF);
  writeRegister(0x28, 0x5B); 

  Serial.print("\nTX at 106.7 MHz...");  

}

void writeRegister(uint8_t qn_register, uint8_t value) {
    tx.setRegister(qn_register, value ); 
    delay(100);
}

void loop() {

}