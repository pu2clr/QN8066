/*
   
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
    currentFrequency = EEPROM.read(1) << 8;
    currentFrequency |= EEPROM.read(2);
    currentRDS = EEPROM.read(3);
    currentMono = EEPROM.read(4);
  } else {
    
  }
  tx.setTX(currentFrequency);
  tx.setRDS(currentRDS);

  showStatus();
}

void showStatus() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.print(tx.formatCurrentFrequency());
  tx.clearRdsBuffer();
}

void showRdsText(uint8_t col, uint8_t lin, char *rdsInfo) {
  oled.setCursor(col, lin);
  oled.clearToEOL();
  oled.setCursor(col, lin);
  oled.print(rdsInfo);
}

/**
 * Processes the RDS Station Name, Station Information, Program information and UTC
 * Runs scrolling to show Station Information and Program information
 */
void processRdsInfo() {

  long currentmillis = millis();
  char aux[22];

  // Shows station name on Display each three seconds.
  if (ps != NULL && (currentmillis - delayPS) > 3000) {
    showRdsText(60, 0, ps);
    delayPS = currentmillis;
  }

  // Shows, with scrolling, station info on display each five seconds.
  if (rt != NULL && strlen(rt) > 1 && (currentmillis - delayRT) > 1000) {
    strncpy(aux, &rt[idxrt], 20);
    aux[20] = 0;
    showRdsText(0, 1, aux);
    idxrt += 2;  // shift left two character
    if (idxrt > 31) idxrt = 0;
    delayRT = currentmillis;
  }

  // Shows, with scrolling, the  program information each a half seconds.
  if (programInfo != NULL && strlen(programInfo) > 1 && (currentmillis - delayProgramInfo) > 500) {
    // Process scrolling
    strncpy(aux, &programInfo[idxProgInfo], 20);
    aux[20] = 0;
    idxProgInfo += 2;  // shift left two character
    showRdsText(0, 2, aux);
    if (idxProgInfo > 60) idxProgInfo = 0;
    delayProgramInfo = currentmillis;
  }
  // Some stations broadcast spurious information. In this case, the displayed time may not make sense.
  if (utcTime != NULL && strlen(utcTime) > 8 && (currentmillis - delayUtcTime) > 60000) {
    showRdsText(0, 3, utcTime);
    delayUtcTime = currentmillis;
  }
}

void loop() {
  uint8_t bkey;
  bkey = ((digitalRead(CMD_UP) << 2) | (digitalRead(CMD_DOWN) << 1)) | digitalRead(CMD_MENU);  // 3, 5 or 6 (Pressed = 0 - considering just one button pressed)
  if (bkey != 0b111) {                                                                             // if none of them is pressed (not igual to 0b011, 0b101 or 0b110) then do nothing.
    if (bkey == 0b011)                                                                             // 3
      tx.seek(RDA_SEEK_WRAP, RDA_CMD_UP, showStatus);
    else if (bkey == 0b101)  // 5
      tx.seek(RDA_SEEK_WRAP, RDA_CMD_DOWN, showStatus);
    else                          // 6
      tx.setMute(!tx.isMuted());  // inverts the audio mute status
    showStatus();
    delay(200);
    // Saves the current frequency if it has changed.
    currentFrequency = tx.getFrequency();
    EEPROM.update(0, VALID_DATA);               // Says that a valid frequency will be saved
    EEPROM.update(1, currentFrequency >> 8);    // stores the current Frequency HIGH byte
    EEPROM.update(2, currentFrequency & 0xFF);  // stores the current Frequency LOW byte
    EEPROM.update(3, tx.isMuted());             // Stores the current audio mute status
  }

  // Queries RDS information.
  if (tx.getRdsAllData(&ps, &rt, &programInfo, &utcTime))
    processRdsInfo();

  delay(5);
}
