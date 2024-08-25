/*
  UNDER CONSTRUCTION...

  ATMEGA328 Dev Modeule version.

  This sketch uses an ATMEGA328 with LCD16X02 DISPLAY
  It is also a FM receiver capable to tune your local FM stations.
  This sketch saves the latest status of the receiver into the Atmega328 eeprom.

  TO RESET the EEPROM: Turn your receiver on with the encoder push button pressed.

  Read more on https://pu2clr.github.io/QN8066/

  Wire up ATMEGA328 Dev Module, QN8066 and LCD16x02 or LCD16x04
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
  | Button                    |                           |               |
  |                           | Menu                      |     A0/D14    |
  | Encoder                   |                           |               |
  |                           | Left                      |     D2        |
  |                           | Right                     |     D3        |
  | --------------------------| --------------------------|---------------|

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  2024.
*/


#include <QN8066.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>


#include "Rotary.h"

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7  4
#define LCD_D6  5
#define LCD_D5  6
#define LCD_D4  7
#define LCD_RS  12
#define LCD_E   13

#define SDIO_PIN 21

// Enconder PINs
// Enconder PINs
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Buttons controllers
#define SWITCH_STEREO  8  // Stereo ON/OFF
#define SEEK_FUNCTION 14  // Seek function

#define POLLING_TIME 1900

#define STORE_TIME 10000  // Time of inactivity to make the current receiver status writable (10s / 10000 milliseconds).
#define PUSH_MIN_DELAY 300


const uint8_t app_id = 66;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;
long storeTime = millis();

uint8_t seekDirection = 1;  // 0 = Down; 1 = Up. This value is set by the last encoder direction.

long pollin_elapsed = millis();


// Encoder control variables
volatile int encoderCount = 0;
uint16_t currentFrequency;
uint16_t previousFrequency;

// Encoder control
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

// LCD display
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

QN8066 rx;

void setup() {

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  // Push button pin
  pinMode(SWITCH_STEREO, INPUT_PULLUP);
  pinMode(SEEK_FUNCTION, INPUT_PULLUP);

  // Start LCD display device
  lcd.begin(16, 2);
  showSplash();


  // If you want to reset the eeprom, keep the ENCODER PUSH BUTTON  pressed during statup
  if (digitalRead(SEEK_FUNCTION) == LOW) {
    EEPROM.write(eeprom_address, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET");
    delay(1500);
    showSplash();
  }

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);

  rx.begin();

  // Check the setXtal function if you are using a active crystal not equal to 32.768 MHz
  // rx.setXtal(1, 1, 0 ); // Using a 32.768 kHz  active crystal instead a 32.768 MHz.  

  delay(100);

  // Checking the EEPROM content
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllReceiverInformation();
  } else {
    currentFrequency = previousFrequency = 939;
  }

  rx.setRX(currentFrequency);
  delay(1000);
  rx.setRxFrequencyStep(2);         // Sets the frequency step to 200 kHz
  rx.setRxFrequencyRange(641,1079); // Sets the FM frequency range from 64,1 MHz to 107.9 MHz

  lcd.clear();

  showStatus();
}


void saveAllReceiverInformation() {
  // The write function/method writes data only if the current data is not equal to the stored data.
  EEPROM.write(eeprom_address, app_id);
  EEPROM.write(eeprom_address + 1, currentFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.write(eeprom_address + 2, currentFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
}

void readAllReceiverInformation() {
  currentFrequency = EEPROM.read(eeprom_address + 1) << 8;
  currentFrequency |= EEPROM.read(eeprom_address + 2);
}


/*
   To store any change into the EEPROM, it is needed at least STORE_TIME  milliseconds of inactivity.
*/
void resetEepromDelay() {
  delay(PUSH_MIN_DELAY);
  storeTime = millis();
  previousFrequency = 0;
}


/*
    Reads encoder via interrupt
    Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
*/
void rotaryEncoder() {  // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
}

void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1000);
}

/*
   Shows frequency information on Display
*/
void showFrequency() {
  currentFrequency = rx.getRxCurrentFrequency();
  lcd.setCursor(4, 1);
  lcd.print(rx.formatCurrentFrequency());
  lcd.display();
}

/*
    Show some basic information on display
*/
void showStatus() {
  showFrequency();
  showStereoMono();
  showRSSI();

  lcd.display();
}

/* *******************************
   Shows RSSI status
*/
void showRSSI() {
  char rssi[12];
  rx.convertToChar(rx.getRxRSSI(), rssi, 3, 0, '.');
  strcat(rssi, "dB");
  lcd.setCursor(13, 1);
  lcd.print(rssi);
}

void showStereoMono() {
  lcd.setCursor(0,1);
  if (rx.isRxStereo()) {
    lcd.print("ST");
  } else {
    lcd.print("MO");
  }
}

/**
   Process seek command.
   The seek direction is based on the last encoder direction rotation.
*/
void doSeek() {

}

void doStereo() {
  
}

void loop() {

  // Check if the encoder has moved.
  if (encoderCount != 0) {
    if (encoderCount == 1) {
      rx.setRxFrequencyUp();
    } else {
      rx.setRxFrequencyDown();
    }
    showStatus();
    encoderCount = 0;
    storeTime = millis();
  }

  if (digitalRead(SWITCH_STEREO) == LOW)
    doStereo();
  else if (digitalRead(SEEK_FUNCTION) == LOW)
    doSeek();

  if ((millis() - pollin_elapsed) > POLLING_TIME) {
    showStatus();
    pollin_elapsed = millis();
  }

  // Show the current frequency only if it has changed
  if ((currentFrequency = rx.getRxCurrentFrequency()) != previousFrequency) {
    if ((millis() - storeTime) > STORE_TIME) {
      saveAllReceiverInformation();
      storeTime = millis();
      previousFrequency = currentFrequency;
    }
  }

  delay(5);
}
