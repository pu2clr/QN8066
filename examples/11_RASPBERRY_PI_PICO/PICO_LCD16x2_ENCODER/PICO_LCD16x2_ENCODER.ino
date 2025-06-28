/*
   QN8066 Arduino Library - Raspberry Pi Pico with LCD16x2 and Rotary Encoder
   
   This sketch demonstrates advanced control of the QN8066 with the Raspberry Pi Pico,
   featuring an LCD display and rotary encoder for interactive frequency control.

   Features:
   - LCD 16x2 display showing frequency, power, and status
   - Rotary encoder for frequency adjustment
   - Push button for menu navigation
   - Real-time audio level monitoring
   - Multiple transmission power levels
   - Stereo/Mono switching

   Author: Ricardo Lima Caratti (PU2CLR) - pu2clr@gmail.com
   December 2024

   Hardware Connections:
   
   Raspberry Pi Pico  |  Component
   -------------------|---------------------------
   3.3V (Pin 36)      |  VCC (QN8066, LCD, etc.)
   GND (Pin 38)       |  GND (Common ground)
   GP0 (Pin 1)        |  SDA (QN8066 and LCD I2C)
   GP1 (Pin 2)        |  SCL (QN8066 and LCD I2C)
   GP2 (Pin 4)        |  Encoder Pin A
   GP3 (Pin 5)        |  Encoder Pin B  
   GP4 (Pin 6)        |  Encoder Button
   GP16 (Pin 21)      |  QN8066 CEN (Optional)
   
   LCD 16x2 I2C:      |  Pico I2C Bus (GP0/GP1)
   QN8066:            |  Pico I2C Bus (GP0/GP1)
   
   External Components:
   - 32.768 kHz active crystal → QN8066 XCLK
   - Antenna → QN8066 RFO
   - Audio source → QN8066 ALI/ARI

   IMPORTANT: This example uses the LiquidCrystal_I2C library
   Install via: Sketch → Include Library → Manage Libraries → Search "LiquidCrystal I2C"
*/

#include <QN8066.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions for Raspberry Pi Pico
#define SDA_PIN 0        // GP0 - I2C SDA
#define SCL_PIN 1        // GP1 - I2C SCL
#define ENCODER_A 2      // GP2 - Encoder pin A
#define ENCODER_B 3      // GP3 - Encoder pin B
#define ENCODER_BTN 4    // GP4 - Encoder button
#define CEN_PIN 16       // GP16 - QN8066 Chip Enable

// LCD I2C address (common addresses: 0x27, 0x3F, 0x20)
#define LCD_ADDRESS 0x27

// Frequency limits (in 0.1 MHz units)
#define MIN_FREQ 875     // 87.5 MHz
#define MAX_FREQ 1080    // 108.0 MHz
#define FREQ_STEP 1      // 0.1 MHz steps

// Power levels
#define MIN_POWER 25
#define MAX_POWER 56
#define POWER_STEP 1

// Objects
QN8066 tx;
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// Variables
volatile int frequency = 1069;  // 106.9 MHz
volatile int txPower = 50;      // Default power
volatile bool stereoMode = true;
volatile int menuMode = 0;      // 0=Freq, 1=Power, 2=Mode
volatile bool encoderButtonPressed = false;

// Encoder variables
volatile int lastEncoderA = 0;
volatile int lastEncoderB = 0;
volatile long lastEncoderTime = 0;

// Display update
unsigned long lastDisplayUpdate = 0;
unsigned long lastStatusUpdate = 0;
bool displayNeedsUpdate = true;

// Custom LCD characters
byte stereoChar[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b01110,
  0b01010,
  0b01010,
  0b00000,
  0b00000
};

byte monoChar[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("QN8066 Pico LCD Control Starting...");
  
  // Initialize I2C
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, stereoChar);
  lcd.createChar(1, monoChar);
  
  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("QN8066 Pico TX");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  // Setup encoder pins
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  
  // Setup interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), buttonISR, FALLING);
  
  // Setup QN8066 chip enable
  if (CEN_PIN >= 0) {
    pinMode(CEN_PIN, OUTPUT);
    digitalWrite(CEN_PIN, HIGH);
    delay(100);
  }
  
  // Detect QN8066
  if (!tx.detectDevice()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("QN8066 ERROR!");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring");
    Serial.println("QN8066 not detected!");
    while (1) delay(1000);
  }
  
  // Initialize QN8066
  tx.setup(1, false, false, 1, 1);
  tx.setTX(frequency);
  //TO DO - Set PWM 
  tx.setTxStereo(stereoMode);
  
  Serial.println("System ready!");
  displayNeedsUpdate = true;
}

void loop() {
  // Handle encoder button press
  if (encoderButtonPressed) {
    encoderButtonPressed = false;
    menuMode = (menuMode + 1) % 3;  // Cycle through modes
    displayNeedsUpdate = true;
    delay(200); // Debounce
  }
  
  // Update display if needed
  if (displayNeedsUpdate || (millis() - lastDisplayUpdate > 1000)) {
    updateDisplay();
    displayNeedsUpdate = false;
    lastDisplayUpdate = millis();
  }
  
  // Update status periodically
  if (millis() - lastStatusUpdate > 3000) {
    updateStatus();
    lastStatusUpdate = millis();
  }
  
  delay(50);
}


void setPower(int power) { 
  if (power < MIN_POWER) power = MIN_POWER;
  if (power > MAX_POWER) power = MAX_POWER;
  txPower = power;
  tx.setPAC(txPower);
} 

void updateDisplay() {
  lcd.clear();
  
  // Line 1: Frequency and status
  lcd.setCursor(0, 0);
  if (menuMode == 0) lcd.print(">");
  lcd.setCursor(1, 0);
  lcd.print(frequency / 10);
  lcd.print(".");
  lcd.print(frequency % 10);
  lcd.print("MHz ");
  
  // Stereo/Mono indicator
  if (stereoMode) {
    lcd.write(0); // Custom stereo character
  } else {
    lcd.write(1); // Custom mono character
  }
  
  // Line 2: Power and mode selection
  lcd.setCursor(0, 1);
  if (menuMode == 1) lcd.print(">");
  lcd.setCursor(1, 1);
  lcd.print("Pwr:");
  lcd.print(txPower);
  
  lcd.setCursor(9, 1);
  if (menuMode == 2) lcd.print(">");
  lcd.setCursor(10, 1);
  lcd.print(stereoMode ? "ST" : "MO");
  
  // Show current menu
  lcd.setCursor(13, 1);
  switch(menuMode) {
    case 0: lcd.print("FRQ"); break;
    case 1: lcd.print("PWR"); break;
    case 2: lcd.print("MOD"); break;
  }
}

void updateStatus() {
  qn8066_status1 status = tx.getStatus1();
  int audioPeak = tx.getAudioPeakValue();
  
  Serial.print("Freq: ");
  Serial.print(frequency / 10.0, 1);
  Serial.print(" MHz | Power: ");
  Serial.print(txPower);
  Serial.print(" | Mode: ");
  Serial.print(stereoMode ? "Stereo" : "Mono");
  Serial.print(" | FSM: ");
  Serial.print(status.arg.FSM);
  Serial.print(" | Audio: ");
  Serial.print(audioPeak);
  Serial.println(" mV");
  
  tx.resetAudioPeak();
}

void encoderISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  // Debounce
  if (interruptTime - lastInterruptTime < 5) return;
  lastInterruptTime = interruptTime;
  
  int currentA = digitalRead(ENCODER_A);
  int currentB = digitalRead(ENCODER_B);
  
  if (currentA != lastEncoderA) {
    if (currentA == currentB) {
      // Clockwise
      switch(menuMode) {
        case 0: // Frequency
          if (frequency < MAX_FREQ) {
            frequency += FREQ_STEP;
            tx.setTX(frequency);
          }
          break;
        case 1: // Power
          if (txPower < MAX_POWER) {
            txPower += POWER_STEP;
            setPower(txPower); 
          }
          break;
        case 2: // Mode
          stereoMode = !stereoMode;
          tx.setTxStereo(stereoMode);
          break;
      }
    } else {
      // Counter-clockwise
      switch(menuMode) {
        case 0: // Frequency
          if (frequency > MIN_FREQ) {
            frequency -= FREQ_STEP;
            tx.setTX(frequency);
          }
          break;
        case 1: // Power
          if (txPower > MIN_POWER) {
            txPower -= POWER_STEP;
            setPower(txPower);
          }
          break;
        case 2: // Mode
          stereoMode = !stereoMode;
          tx.setTxStereo(stereoMode);
          break;
      }
    }
    displayNeedsUpdate = true;
  }
  
  lastEncoderA = currentA;
  lastEncoderB = currentB;
}

void buttonISR() {
  static unsigned long lastButtonTime = 0;
  unsigned long buttonTime = millis();
  
  // Debounce
  if (buttonTime - lastButtonTime < 300) return;
  lastButtonTime = buttonTime;
  
  encoderButtonPressed = true;
}
