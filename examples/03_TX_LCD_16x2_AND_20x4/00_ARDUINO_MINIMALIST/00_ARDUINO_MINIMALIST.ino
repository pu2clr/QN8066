/*
  This sketch presents a "minimal" version of an FM transmitter with RDS support 
  and displays some information on a 16x2 LCD. In this version, there are no 
  button or encoder controls for changing transmitter configuration parameters 
  at runtime. The idea behind this sketch is to serve as a starting point for 
  the user to develop their own interface.

  Read more on https://pu2clr.github.io/QN8066/

  Wire up on Arduino UNO, Nano or Pro mini

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  |    LCD 16x2 or 20x4       |                           |               |
  |                           | D4                        |     D7        |
  |                           | D5                        |     D6        |
  |                           | D6                        |     D5        |
  |                           | D7                        |     D4        |
  |                           | RS                        |     D12       |
  |                           | E/ENA                     |     D13       |
  |                           | RW & VSS & K (16)         |    GND        |
  |                           | A (15) & VDD              |    +Vcc       |
  | --------------------------| ------------------------- | --------------|
  | QN8066                    |                           |               |
  | DIY KIT 5˜7W              | ------------------------- | --------------|
  |                           | SDA                       |     A4        |
  |                           | SCLK                      |     A5        |
  | --------------------------| --------------------------|---------------|



  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Aug, 2024.
*/

#include <QN8066.h>
#include <LiquidCrystal.h>

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

uint8_t pty = 5; // Program Type - "Education" in RDS and "Rock" in RBDS see: https://en.wikipedia.org/wiki/Radio_Data_System
uint16_t txFrequency = 1069;  // 106.9 MHz

// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  lcd.begin(16, 2);
  showSplash();
  lcd.clear();

  if (!tx.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1) 
      ;
  }
  tx.setup();
  tx.setTX(txFrequency); // Sets the trasmitter to 106.9 MHz
  tx.rdsTxEnable(true);
  tx.rdsInitTx(0, 0, 0, pty, 30, 8);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html)
  showStatus();

  delay(1000);
}

void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1500);
}

void showStatus() {
  char strFrequency[12];
  // lcd.clear();
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 0);
  lcd.print(strFrequency);
  lcd.print("MHz");
  lcd.display();
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