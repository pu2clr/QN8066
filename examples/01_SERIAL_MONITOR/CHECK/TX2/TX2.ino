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

  delay(2000);
  tx.setup();
  delay(1000);  
  Serial.begin(9600);
  while (!Serial);  
  Serial.print("\nStarting TX...");
  writeRegister(0x00, 0B11100011); // SYSTEM1 => 11100011  =>  swrst = 1; recal = 1; stnby = 1; ccs_ch_dis = 1; cca_ch_dis = 1
  writeRegister(0x01, 0B00000000); // SYSTEM2 => 00000000  => Pre-emphasis and de-emphasis time constant = 50 us
  writeRegister(0x02, 0B01010000); // CCA => 01010000 => xtal_inj = 0; imr = 1; SNR_CCA_TH = 0
  writeRegister(0x07, 0B11101000); // XTAL_DIV0 => 11101000 
  writeRegister(0x08, 0B00001011); // XTAL_DIV1 => 00001011 => Divider = 01111101000 => 1000 (em Decimal)
  writeRegister(0x09, 0B01011100); // XTAL_DIV2 = > 01011100 (It is the default value)
  writeRegister(0x19, 0B00100010); // INT_CTRL => 00100010 (It is the  default value) => TXCH Highest 2 bits = 10

  writeRegister(0x00, 0B00001011); // SYSTEM1 => 00001011 => Set TX On
  writeRegister(0x25, 0B01111101); // FDEV => 01111101 => 125 (Decimal)
  writeRegister(0x26, 0B00111100); // RDS => 00111100 => Line_in_en = 0; RDSFDEV = 60 (Decimal) 
  writeRegister(0x27, 0B00111001); // GPLT => 00111001 => Tx_sftclpth = 00 (12’d2051 - 3db back off from 0.5v); t1m_sel = 11 (Infinity); GAIN_TXPLT = 1001 (9% 75 kHz)

  writeRegister(0x00, 0B00001011); // SYSTEM1 => 00001011 => txreq = 1; ccs_ch_dis = 1; cca_ch_dis = 1 

  writeRegister(0x19, 0B00100011); // INT_CTRL => 00100011 =>  rds_only = 1; TXCH (Highest 2 bits) = 11
  writeRegister(0x1B, 0B10100111); // TXCH => 10100111  => Channel = 1110100111 => 935 
                                   // Freq = (X - 60) / 0,05 => 935 * 0,05 + 60 = 106,75 MHz
  writeRegister(0x49, 0B11101000); // ???????  = 11101000 => 
  writeRegister(0x1B, 0B10100110); // TXCH => 10100110 

  writeRegister(0x49, 0B11011111); // ??? 11011111

  writeRegister(0x6E, 0B11111111); // ???  
  writeRegister(0x28, 0B01011011); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)
  Serial.print("\nTX at 106.7 MHz...");  

}

void writeRegister(uint8_t qn_register, uint8_t value) {
    tx.setRegister(qn_register, value ); 
}

void loop() {

}