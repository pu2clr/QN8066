/*
  UNDER CONSTRUCTION... 
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD16X02.

  ABOUT THE ATMEGA328 EEPROM and saving the receiver current information
  ATMEL says the lifetime of an EEPROM memory position is about 100,000 writes.
  For this reason, this sketch tries to avoid unnecessary writes into the eeprom.
  This firmware saves the latest setup parameters.

  TO RESET the EEPROM: Turn your receiver on with the MENU push button pressed.

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
  |                           | PWM                       |     D9        |
  | --------------------------| --------------------------| --------------|
  | Buttons                   |                           |               |
  |                           | Menu                      |      8        |
  |                           | Left (Down / -)           |     10        |
  |                           | Right (Up / + )           |     11        |
  |                           | RESET                     |      3        |
  | --------------------------| --------------------------|---------------|


  See user_manual.txt

  Prototype documentation: https://pu2clr.github.io/RDA5807/
  PU2CLR RDA5807 API documentation: https://pu2clr.github.io/RDA5807/extras/apidoc/html/

  By PU2CLR, Ricardo,  Feb  2023.
*/

#include <QN8066.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

// Enconder PINs

#define BT_RESET    3
#define BT_MENU     8
#define BT_UP      10
#define BT_DOWN    11
#define PWM_PA      9


#define PUSH_MIN_DELAY 300

const uint8_t app_id = 43;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;
long storeTime = millis();

// LCD display
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

uint16_t txFrequency = 1069;  // Default frequency is 106.9 MHz
bool bRds = false;
bool bStereo = true;
uint8_t inputInpedance = 2; // Default 20 KOhms

bool bShow = false;

QN8066 tx;

void setup() {

  lcd.begin(16, 2);

  showSplash();

  // Checking the EEPROM content
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllReceiverInformation();
  } 

  tx.setup();

  tx.setTX(txFrequency);

  lcd.clear();
  showStatus();
}

void saveAllReceiverInformation() {
  // The update function/method writes data only if the current data is not equal to the stored data.
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, inputInpedance);           // stores the current inputInpedance
  EEPROM.update(eeprom_address + 2, txFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 3, txFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 4, (uint8_t)bRds);
  EEPROM.update(eeprom_address + 5, (uint8_t)bStereo);
  // TODO
}

void readAllReceiverInformation() {
  inputInpedance =  EEPROM.read(eeprom_address + 1);
  txFrequency = EEPROM.read(eeprom_address + 2) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 3);
  bRds = (bool)EEPROM.read(eeprom_address + 4);
  bStereo = (bool)EEPROM.read(eeprom_address + 5);
  // TODO
}


void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1000);
}

/*
   Shows frequency information on Display
*/
void showFrequency() {
  // TODO
  lcd.setCursor(0, 0);
  lcd.display();
}

/*
    Show some basic information on display
*/
void showStatus() {
  char strFrequency[7];

  tx.convertToChar(txFrequency, strFrequency, 5, 3, ','); // Convert the selected frequency a array of char 

  lcd.setCursor(0,0);
  lcd.print("TXing at ");
  lcd.print(strFrequency);
  lcd.print("MHz");

  lcd.display();
}

void showStereoMono() {
  lcd.setCursor(0, 1);
  if (bStereo) {
    lcd.print("ST");
  } else {
    lcd.print("MO");
  }
}

void showRds() {
  // TODO
}


/*********************************************************

 *********************************************************/

void doStereo() {
  bStereo = !bStereo;
  bShow = true;
  showStereoMono();
}

void doRds() {
  bRds = !bRds;
  showRds();
}

void loop() {


  delay(5);
}
