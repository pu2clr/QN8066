// Build an FM transmitter with RDS in 16 lines of code using the QN8066 Arduino Library
#include <QN8066.h>
char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";
QN8066 tx;
void setup() {
 tx.setup();
 tx.setTX(1069);   // Sets frequency to 106.9 MHz 
 tx.rdsInitTx(0x8,0x1,0x9B, 50, 20);// Sets PI code, sync delay and more     
 tx.rdsSetPTY(5); // Program type is EDUCATION if you are using RDS or ROCK if you are using RBDS.
 tx.rdsTxEnable(true);  // Starts RDS service   
}
void loop() {
    tx.rdsSendPS(ps);
    tx.rdsSendRTMessage(rt);
    delay(3000);    
}
