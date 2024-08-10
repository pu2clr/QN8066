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
    2) Setup: Chip: ATtiny85;  Clock Source: 4MHz (Internal); LTO Enabled; millis() / macros() Enabled; 

    ATTENTION: if you select Clock source 8 MHz, for some reason, the system will work very slow. Maybe a bug. 

   By Ricardo Lima Caratti, 2024.
*/

#include <QN8066.h>
#include <EEPROM.h>  // The ATtiny85 contains 512 bytes of data EEPROM memory. The EEPROM has an endurance of at least 100,000 write/erase cycles.
#include <Tiny4kOLED.h>
#include <5x5_font.h>

#define CMD_UP PB1
#define CMD_DOWN PB4
#define CMD_MENU PB3
#define TIME_RDS_ON_DISPLAY 2300

#define VALID_DATA 85

char ps[] = "QN8066TX";
char rt[] = "PU2CLR QN8066 ARDUINO LIBRARY";

long delayPS = millis();
long delayRT = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz is the default frequency
uint8_t currentRDS = 0;           // Default no RDS 
uint8_t currentMono = 0;          // Default Stereo

QN8066 tx;

void setup() {
  pinMode(CMD_UP, INPUT_PULLUP);
  pinMode(CMD_DOWN, INPUT_PULLUP);
  pinMode(CMD_MENU, INPUT_PULLUP);

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

  // Restores the latest frequency and audio mute statis saved into the EEPROM
  if (EEPROM.read(0) == VALID_DATA) {
     readEEPROM(); 
  } else {
    writeEEPROM();    // Writes into EEPROM the default values 
  }
  tx.setTX(currentFrequency);
  tx.rdsTxEnable(currentRDS);
  tx.setTxMono(currentMono);    //  1 = Mono; 0 = Stereo 

  showStatus();
}

// Write/update current information into EEPROM
void writeEEPROM() {
    EEPROM.update(0, VALID_DATA);               // Says that a valid frequency will be saved
    EEPROM.update(1, currentFrequency >> 8);    // stores the current Frequency HIGH byte
    EEPROM.update(2, currentFrequency & 0xFF);  // stores the current Frequency LOW byte
    EEPROM.update(3, currentRDS);                
    EEPROM.update(3, currentMono);                
}  
// Rescue the privious information from EEPROM
void readEEPROM() {
    currentFrequency = EEPROM.read(1) << 8;
    currentFrequency |= EEPROM.read(2);
    currentRDS = EEPROM.read(3);
    currentMono = EEPROM.read(4);
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
  /* 
  uint8_t bkey;
  bkey = ((digitalRead(CMD_UP) << 2) | (digitalRead(CMD_DOWN) << 1)) | digitalRead(CMD_MENU);  // 3, 5 or 6 (Pressed = 0 - considering just one button pressed)
  if (bkey != 0b111) {                                                                             // if none of them is pressed (not igual to 0b011, 0b101 or 0b110) then do nothing.
    if (bkey == 0b011)                                                                             // 3
      // UP
    else if (bkey == 0b101)  // 5
      // UP
    else                     // 6
      // MENU
    showStatus();
    delay(200);
    // Saves the current frequency if it has changed.
    currentFrequency = tx.getFrequency();
    EEPROM.update(0, VALID_DATA);               // Says that a valid frequency will be saved
    EEPROM.update(1, currentFrequency >> 8);    // stores the current Frequency HIGH byte
    EEPROM.update(2, currentFrequency & 0xFF);  // stores the current Frequency LOW byte
    EEPROM.update(3, tx.isMuted());             // Stores the current audio mute status
  }
  */

  delay(5);
}
