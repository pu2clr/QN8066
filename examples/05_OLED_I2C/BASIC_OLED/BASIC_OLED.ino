/*
  This sketch is an example of using the QN8066 library for Arduino to run on the Arduino based on ATMega328 and OLED. 
  See documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html 
  By Ricardo Lima Caratti, 2024.
*/

#include <QN8066.h>
#include <Tiny4kOLED.h>
#include <5x5_font.h>

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz is the default frequency

// Station Name (PS) messages
char *ps[] = { (char *)"PU2CLR \r",
               (char *)"QN8066 \r",
               (char *)"ARDUINO\r",
               (char *)"LIBRARY\r",
               (char *)"FM TX  \r" };
      
// Radio Text (RT) messages
// Apparently, some receivers seem to work better when the special character 0xD (\r) 
// is added at the end of the string (message). Therefore, although it is technically 
// possible to work with messages up to 64 characters long, it is recommended to keep 
// the message length within 32 characters by default, reserving the 32nd character for
// the '\r' as shown below.
char *rt[] = { (char *)"PU2CLR QN8066 ARDUINO LIBRARY  \r",
               (char *)"FM TRANSMITTER WITH RDS SERVICE\r",
               (char *)"github.com/pu2clr/QN8066       \r",
               (char *)"FM Transmitters Enthusiasts    \r",
               (char *)"QN8066 HOMEBREW FM TRANSMITTER \r" };

const uint8_t lastPS = ( sizeof(ps) / sizeof(ps[0])) - 1;
const uint8_t lastRT = ( sizeof(rt) / sizeof(rt[0])) - 1;                    

uint8_t idxPS = 0;
uint8_t idxRT = 0;

long timePS = millis();
long timeRT = millis();
long timeDateTime = millis();


QN8066 tx;

void setup() {
  oled.begin();
  oled.on();
  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(F("QN8066-ATTiny85"));
  oled.setCursor(0, 2);
  oled.print(F("   By PU2CLR   "));
  delay(2000);
  oled.clear();
  // End Splash

  tx.setup();
  tx.setTX(currentFrequency);
  tx.setTxMono(false);   //  Stereo
  tx.rdsSetSyncTime(30); // 30 ms of delay time - Needed due to the low speed that the Attiny was configured (1MHz clock)
  tx.rdsSetRepeatSendGroup(20); // Repeat 20 times each RDS block sent to the QN8066
  tx.rdsSetPTY(5); // Program type is EDUCATION if you are using RDS or ROCK if you are using RBDS. See: https://en.wikipedia.org/wiki/Radio_Data_System
  tx.rdsTxEnable(true);
  showStatus();
}

void sendRDS() {

  // PS refreshing control
  if ((millis() - timePS) > RDS_PS_REFRESH_TIME) {
    if (idxPS > lastPS) idxPS = 0;
    delay(100);
    tx.rdsSendPS(ps[idxPS]);
    idxPS++;
    timePS = millis();
  }

  // RT refreshing control
  if ((millis() - timeRT) > RDS_RT_REFRESH_TIME) {
    if (idxRT > lastRT) idxRT = 0;
    delay(100);
    tx.rdsSendRT(rt[idxRT]);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    idxRT++;
    timeRT = millis();
  }

  // Date Time Service refreshing control
  if ((millis() - timeDateTime) > RDS_DT_REFRESH_TIME) {
    delay(100);
    // To use the function (service) below, you will need to add an integrated clock to your 
    // system that provides the date and time to the system. The following example presents 
    // only a fixed date and time and is intended solely to illustrate the use of the function.
    tx.rdsSendDateTime(2024, 8, 30, 13, 01, 0);  // Sends Year = 2024; month = 8; day = 29; At 12:45 (local time)    
    timeDateTime = millis();
  }
}


// Shows the current status
void showStatus() {
  char strFrequency[7];
  tx.convertToChar(currentFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  oled.clear();
  oled.setCursor(0, 0);
  oled.print(strFrequency);
}

void loop() {
  sendRDS();
  delay(800);
}
