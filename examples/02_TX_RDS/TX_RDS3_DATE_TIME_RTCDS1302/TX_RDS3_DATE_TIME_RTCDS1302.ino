#include <QN8066.h>

#include <Ds1302.h>

// RTC DS1302 control 
#define CLK_PIN     14      // Using the D14/A0 for clock
#define DATA_PIN    15      // Using the D15/A1 for data
#define RESET_PIN   16      // Using the D16/A2 for reset the DS1302 device

char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";
QN8066 tx;


Ds1302 rtc(RESET_PIN, CLK_PIN, DATA_PIN);
Ds1302::DateTime dt;

void setup() {
 rtc.init(); 
 tx.setup(1000,false,true);
 tx.setTX(1069);   // Sets frequency to 106.9 MHz 
 // tx.rdsEnableRX(true);
 tx.rdsInitTx(0x8,0x1,0x9B, 8, 50, 20);// Sets PI code, sync delay and more   

  // To set the RTC, uncomment this block of lines, compile and upload the sketch to the Arduino. 
  // Once the clock is set, comment out the lines again, compile and upload the sketch.  
  
  /*
  dt.year = 24;
  dt.month = 8;
  dt.day = 29;
  dt.dow = 1;
  dt.hour = 23;
  dt.minute = 16;
  dt.second = 30;

  rtc.setDateTime(&dt);
  
  */

}
void loop() {
    rtc.getDateTime(&dt);
    tx.rdsSendPS(ps);
    tx.rdsSendRTMessage(rt);
    // To use the function (service) below, you will need to add an integrated clock to your 
    // system that provides the date and time to the system. This example uses the RTC DS1302 device
    if (dt.dow)
      tx.rdsSendDateTime(dt.year + 2000, dt.month, dt.day, dt.hour, dt.minute, 0);

    delay(10000);    
}