/*
  This sketch is an example of using the QN8066 library for Arduino to run on the ATtiny85 and OLED. Learn more details below.
  It is important to highlight some limitations regarding the number of pins available on the ATtiny85. This condition somewhat 
  restricts the user interface. 

  ATtiny85, OLED and QN8066 wireup  
  | Device pin      | Attiny85 REF pin | Physical pin  | 
  | ----------------| -----------------| ------------- | 
  | PUSH BUTTON     |                  |               |
  | UP              |     PB1          |     6         | 
  | DOWN            |     PB4          |     3         |
  | MENU            |     PB3          |     2         | 
  |                 |                  |               |
  | QN8066 & OLED   |                  |               | 
  | SDIO / SDA      |     SDA          |     5         |
  | SCLK / CLK      |     SCL          |     7         |


  Compiling and uploading: 
  1) Install the ATtiny Core in Arduino IDE - Insert the URL http://drazzy.com/package_drazzy.com_index.json on board manager. 
                                              To do that, go to Preferences, enter the above URL in "Additional Boards Manager URLs. 
                                              To setup ATtiny85 on Arduino IDE, go to Tools Menu, Board, Board Manager and install 
                                              "ATTinyCore by Spence Konde". 
  2) Setup: Chip: ATtiny85;  Clock Source: 1MHz (Internal); LTO Enabled; millis() / macros() Enabled; 
  ATTENTION: if you select Clock source 8 MHz, for some reason, the system will work very slow. Maybe a bug. 
  See documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html 
  By Ricardo Lima Caratti, 2024.
*/

#include <QN8066.h>
#include <Tiny4kOLED.h>
#include <5x5_font.h>

char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";

uint16_t currentFrequency = 1069; // 106.9 MHz is the default frequency

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
  tx.rdsSetSyncTime(30); // Needed due to the low speed that the Attiny was configured (1MHz clock)
  tx.rdsTxEnable(true);
  showStatus();
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
  tx.rdsSendPS(ps,20);
  delay(200);
  tx.rdsSendRTMessage(rt,30);
  delay(800);
}
