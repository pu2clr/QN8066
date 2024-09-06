/*
  This sketch uses an Arduino Nano with NOKIA 5110 display. 
  It is a FM receiver with RDS feature capable to tune your local FM stations.
  This sketch saves the latest status of the receiver into the Atmega328 eeprom (frequency, RDS and Stereo setup).
  
  Read more on https://pu2clr.github.io/QN8066/

  Wire up on Arduino UNO, Nano or Pro mini

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  | NOKIA 5110                |                           |               |
  |                           | (1) RST (RESET)           |     8         |
  |                           | (2) CE or CS              |     9         |
  |                           | (3) DC or DO              |    10         |
  |                           | (4) DIN or DI or MOSI     |    11         |
  |                           | (5) CLK                   |    13         |
  |                           | (6) VCC  (3V-5V)          |    +VCC       |
  |                           | (7) BL/DL/LIGHT           |    +VCC       |
  |                           | (8) GND                   |    GND        |
  | --------------------------| ------------------------- | --------------|
  | QN8066                    |                           |               | 
  |                           | +Vcc 3.3V [1]             |   3.3V        |    
  |                           | SDIO (pin 8)              |     A4        |
  |                           | SCLK (pin 7)              |     A5        |
  | --------------------------| --------------------------| --------------|

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR BK1088 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Sep  2024.
*/

#include <QN8066.h>

#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_PCD8544.h>  // See: https://www.electronoobs.com/eng_arduino_Adafruit_PCD8544.php
#include <SPI.h>

// NOKIA Display pin setup
#define NOKIA_RST 8   // RESET
#define NOKIA_CE 9    // Some NOKIA devices show CS
#define NOKIA_DC 10   //
#define NOKIA_DIN 11  // MOSI
#define NOKIA_CLK 13  // SCK
#define NOKIA_LED 0   // 0 if wired to +3.3V directly

#define SDA_PIN A4 // SDA pin used by your Arduino Board
#define CLK_PIN A5

int maxX1;
int maxY1;

uint8_t pty = 5; // Program Type - "Education" in RDS and "Rock" in RBDS see: https://en.wikipedia.org/wiki/Radio_Data_System
uint16_t txFrequency = 1069;  // 106.9 MHz

// Nokia 5110 display
Adafruit_PCD8544 display = Adafruit_PCD8544(NOKIA_DC, NOKIA_CE, NOKIA_RST);
QN8066 tx;

void setup() {

  // Start the Nokia display device
  display.begin();
  // ATTENTION: YOU MUST VERIFY THE BEST LAVEL FOR THE CONTRAST OF YOUR DISPLAY.
  display.setContrast(50);  // You may need adjust this value for you Nokia 5110
  showSplash();
  showTemplate();

  tx.setup();
  tx.setTX(txFrequency); // Sets the trasmitter to 106.9 MHz
  tx.rdsTxEnable(true);
  tx.rdsInitTx(0, 0, 0, pty, 30, 8);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html)
  showStatus();
  delay(1000);
}


void showSplash() {
  display.clearDisplay();
  display.display();
  display.setTextColor(BLACK);
  // Splash - Change it by the your introduction text.
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("QN8066");
  display.setCursor(0, 15);
  display.print("Arduino");
  display.setCursor(0, 30);
  display.print("Library");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

/*
   Shows the static content on  display
*/
void showTemplate() {

  maxX1 = display.width() - 2;
  maxY1 = display.height() - 2;

  // TO DO: The frame of the screen
}


/*
   Shows frequency information on Display
*/
void showFrequency() {
 char strFrequency[8];  
  display.setCursor(3, 8);
  display.setTextSize(2);
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  display.print(strFrequency);  
  display.display();
}

/*
    Show some basic information on display
*/
void showStatus() {
  display.fillRect(0, 0, 84, 23, WHITE);
  display.setTextColor(BLACK);
  showFrequency();
  display.display();
}

void loop() {
  tx.rdsSendPS((char *) "PU2CLR \r");
  delay(2000);
  tx.rdsSendRT((char *) "QN8066 ARDUINO LIBRARY       \r" );
  delay(15000);
  tx.rdsSendPS((char *) "QN8066 \r");
  delay(2000);
  tx.rdsSendRT((char *) "FM TRANSMITTER WITH RDS      \r" );
  delay(15000);
}






