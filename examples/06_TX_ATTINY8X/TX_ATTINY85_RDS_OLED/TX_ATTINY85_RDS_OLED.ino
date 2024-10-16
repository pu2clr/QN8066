/*
  This sketch is an example of using the QN8066 library for Arduino to run on the ATtiny85 and OLED. Learn more details below.
  It is important to highlight some limitations regarding the number of pins available on the ATtiny85. This condition somewhat 
  restricts the user interface. 

  ATtiny85, OLED and QN8066 wireup  
  | Device pin      | Attiny85 REF pin | Physical pin  | 
  | ----------------| -----------------| ------------- | 
  | ENCODER.        |                  |               |
  | A               |     PB1          |     6         | 
  | B               |     PB4          |     3         |
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
#include "Rotary.h"


#define ENCODER_PIN_A PB1
#define ENCODER_PIN_B PB4
#define BT_MENU PB3


char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";

uint16_t currentFrequency = 1069; // 106.9 MHz is the default frequency

volatile int encoderCount = 0;

Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

QN8066 tx;

void setup() {

  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);  


  // controlling encoder via interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);



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


/**
 * Reads encoder via interrupt
 * Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
 */
void rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
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
  tx.rdsSendPS(ps);
  delay(200);
  tx.rdsSendRTMessage(rt);
  delay(800);
}
