#include <QN8066.h>
QN8066 tx;
void setup() {
    tx.begin();
    // 
    tx.setRegister(0x00,0xE3);  // 11100011 - swrst, recal, stnby 0, 0, 0, ccs_ch_dis, cca_ch_dis
    tx.setRegister(0x01,0x00);  // 00000000 - 
    // Sine wave clock = 0; imr = 1;  SNR_CCA_TH = 010000
    tx.setRegister(0x02,0x50);  // 01010000
    //  
    tx.setRegister(0x07,0xE8);  // 11101000 
    tx.setRegister(0x08,0x0B);  // 00001011 ==> 011 11101000 = 1000 => XTAL DIVIDER
    tx.setRegister(0x09,0x5C);  // 01011100 = OK
    tx.setRegister(0x19,0x22);  // 00100010 == rds_int_en = 0; cca_int_en = 0; rds_only =1 
    tx.setRegister(0x1B,0xF8);  // 11111000
    tx.setRegister(0x00,0x0B);  // 00001011
    tx.setRegister(0x25,0x7D);  // 01111101
    tx.setRegister(0x26,0x3C);  // 00111100
    tx.setRegister(0x27,0x39);  // 00111001
    tx.getRegister(0x00);       // Read = 0x0B
    tx.setRegister(0x00,0x0B);  // 00001011
    tx.getRegister(0x19);       // Read = 0x22
    tx.setRegister(0x19,0x23);  // 00100011
    tx.setRegister(0x1B,0xAB);  // 10101011
    tx.getRegister(0x49);       // Read = 0x05
    tx.setRegister(0x49,0x25);  // 00100101
    tx.setRegister(0x1B, 0xAA); // 10101010
    tx.getRegister(0x49);       // Read = 0x25
    tx.setRegister(0x49, 0x05); // 00000101
    tx.getRegister(0x6E);       // Read = 0x37
    tx.getRegister(0x6E);       // Read = 0x37
    tx.setRegister(0x6E,0xB7);  // 10110111
    tx.getRegister(0x6E);       // Read = 0xB7
    tx.setRegister(0x6E, 0x37); // 00110111
    tx.getRegister(0x6E);       // Read = 0x37 
    tx.setRegister(0x6E,0xB7);  // 10110111
    tx.setRegister(0x28,0x4B);  // 01001011
    tx.getRegister(0x00);       // Read = 0x0B 
    tx.setRegister(0x00,0x0B);  // 00001011
    tx.getRegister(0x19);       // Read = 0x23 
    tx.setRegister(0x19,0x23);  // 00100011
    tx.setRegister(0x1B,0xAD);  // 10101101
    tx.getRegister(0x49);       // Read = 0x05                                 
    tx.setRegister(0x49,0x25);  // 00100101
    tx.setRegister(0x1B,0xAC);  // 10101100    
    tx.getRegister(0x49);       // Read = 0x25    
    tx.setRegister(0x49,0x05);  // 00000101   
    tx.getRegister(0x6E);       // Read = 0xB7 
    tx.getRegister(0x00);       // Read = 0x0B
    tx.setRegister(0x00,0x0B);  // 00001011
    tx.getRegister(0x19);       // Read = 0x23 
    tx.setRegister(0x19,0x23);  // 00100011
    tx.setRegister(0x1B,0xAF);  // 10101111
    tx.getRegister(0x49);       // Read = 0x05 
    tx.setRegister(0x49,0x25);  // 00100101
    tx.setRegister(0x1B,0xAE);  // 10101110
    tx.getRegister(0x49);       // Read = 00100101
    tx.setRegister(0x49,0x05);  // 00000101
    tx.getRegister(0x6E);       // Read = 10110111 
}
void loop () {
  delay(5);
}
