#include <QN8066.h>
QN8066 tx;
void setup() {
    tx.begin();
    tx.setRegister(0x00,0xE3); 
    tx.setRegister(0x01,0x00);
    tx.setRegister(0x02,0x50);
    tx.setRegister(0x07,0xE8);
    tx.setRegister(0x08,0x0B);
    tx.setRegister(0x09,0x5C);
    tx.setRegister(0x19,0x22);
    tx.setRegister(0x1B,0xF8);
    tx.setRegister(0x00,0x0B);
    tx.setRegister(0x25,0x7D);
    tx.setRegister(0x26,0x3C);
    tx.setRegister(0x27,0x39);
    tx.getRegister(0x00);       // Read = 0x0B
    tx.setRegister(0x00,0x0B);
    tx.getRegister(0x19);       // Read = 0x22
    tx.setRegister(0x19,0x23);
    tx.setRegister(0x1B,0xAB);
    tx.getRegister(0x49);       // Read = 0x05
    tx.setRegister(0x49,0x25);   
    tx.setRegister(0x1B, 0xAA); 
    tx.getRegister(0x49);       // Read = 0x25
    tx.setRegister(0x49, 0x05);   
    tx.getRegister(0x6E);       // Read = 0x37
    tx.getRegister(0x6E);       // Read = 0x37
    tx.setRegister(0x6E,0xB7);  
    tx.getRegister(0x6E);       // Read = 0xB7
    tx.setRegister(0x6E, 0x37);  
    tx.getRegister(0x6E);       // Read = 0x37 
    tx.setRegister(0x6E,0xB7); 
    tx.setRegister(0x28,0x4B); 
    tx.getRegister(0x00);       // Read = 0x0B 
    tx.setRegister(0x00,0x0B); 
    tx.getRegister(0x19);       // Read = 0x23 
    tx.setRegister(0x19,0x23); 
    tx.setRegister(0x1B,0xAD); 
    tx.getRegister(0x49);       // Read = 0x05                                 
    tx.setRegister(0x49,0x25);    
    tx.setRegister(0x1B,0xAC);    
    tx.getRegister(0x49);       // Read = 0x25    
    tx.setRegister(0x49,0x05);    
    tx.getRegister(0x6E);       // Read = 0xB7 
    tx.getRegister(0x00);       // Read = 0x0B
    tx.setRegister(0x00,0x0B); 
    tx.getRegister(0x19);       // Read = 0x23 
    tx.setRegister(0x19,0x23); 
    tx.setRegister(0x1B,0xAF); 
    tx.getRegister(0x49);       // Read = 0x05 
    tx.setRegister(0x49,0x25); 
    tx.setRegister(0x1B,0xAE); 
    tx.getRegister(0x49);       // Read = 25
    tx.setRegister(0x49,0x05); 
    tx.getRegister(0x6E);       // Read = B7 
}
void loop () {
  delay(5);
}