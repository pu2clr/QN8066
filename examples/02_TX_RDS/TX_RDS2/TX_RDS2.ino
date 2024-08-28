#include <QN8066.h>
char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";
QN8066 tx;
void setup() {
 tx.setup(1000,false,true);
 tx.setTX(1069);   // Sets frequency to 106.9 MHz 
 tx.rdsEnableRX(true);
 tx.rdsInitTx(0x8,0x1,0x9B, 8, 50, 20);// Sets PI code, sync delay and more     
}
void loop() {
    tx.rdsSendPS(ps);
    tx.rdsSendRTMessage(rt);
    tx.rdsSendDateTime(2024, 8, 28, 23, 01, 0);
    delay(10000);    
}