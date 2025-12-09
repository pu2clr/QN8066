/*
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD20X04.
  
  ** PROTEUS 8 SIMULATION VERSION **
  This version uses POLLING instead of INTERRUPTS for encoder reading,
  making it compatible with Proteus 8 simulator.

  This sketch has been successfully tested on Arduino Uno, Nano, Pro Mini, and LGT8F328 boards.

  ABOUT THE ATMEGA328 EEPROM and saving the receiver current information
  ATMEL says the lifetime of an EEPROM memory position is about 100,000 writes.

  ATTENTION: TO RESET the EEPROM: Turn your receiver on with the MENU push button pressed.

  Read more at https://pu2clr.github.io/QN8066/

  ATTENTION: Preferably use an Arduino that operates at 3.3V instead of an 
             Arduino that operates at 5V. Consider adding two 10K pull-up 
             resistors to the I2C bus to improve system stability.
             If you are using a 5V Arduino, consider adding two more 150R r
             esistors in series with the I2C bus. See the 'schematic diagram' 
             posted in this repository for this example for more details.


  This program uses the Arduino EEPROM to store the transmitter configuration parameters. 
  Below are some operational commands:
  1. If necessary, press the MENU button when turning on the system to reset the EEPROM and 
     return the transmitter to the default settings.
  2. On the main screen, turn the encoder clockwise and counterclockwise to display information pages about the 
     current state of the transmitter.
  3. Press the MENU button (encoder push button) to enter the parameter navigation mode.
  3.1. At this level, turn the encoder clockwise and counterclockwise to select the option or parameter you want to change.
  3.2. Press the push button again to enter parameter editing mode.
  3.3. turn the encoder clockwise and counterclockwise to change the parameters.
  3.4. Press the push button button to confirm and save the selected parameter. At this point, the
       menu returns to the previous level, allowing you to select another parameter to change.
  4. The "Main Screen" option returns the system to the initial screen. Remember that at this 
    point, you can change pages by turn the encoder clockwise and counterclockwise This way, you can check the changes 
    made.     

  Check the RDS functions documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html

  Wire up on Arduino UNO, Nano or Pro mini

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  |    LCD 16x2 or 20x4       |                           |               |
  |                           | D4                        |      7        |
  |                           | D5                        |      6        |
  |                           | D6                        |      5        |
  |                           | D7                        |      4        |
  |                           | RS                        |      9        |
  |                           | E/ENA                     |      8        |
  |                           | RW & VSS & K (16)         |     GND       |
  |                           | A (15) & VDD              |    +Vcc       |
  | --------------------------| ------------------------- | --------------|
  | QN8066                    |                           |               |
  | DIY KIT 5˜7W              | ------------------------- | --------------|
  |                           | SDA                       |     A4        |
  |                           | SCLK                      |     A5        |
  |                           | PWM                       |     10        |
  | --------------------------| --------------------------| --------------|
  | Buttons                   |                           |               |
  |                           | Menu                      |     12        |
  |                           | Left (Down / -)           |      3        |
  |                           | Right (Up / + )           |      2        |
  | --------------------------| --------------------------|---------------|

  IMPORTANT: 
  There are some important considerations when using a microcontroller like the 
  Arduino Nano with the "DIY 5~7W FM Transmitter Kit". This kit is designed to 
  work with a 3.3V controller, meaning the I2C bus is configured with pull-up 
  resistors connected to the board's power supply. However, the Arduino Nano 
  operates at 5V, producing a 5V signal on the I2C bus, which can cause 
  instability in I2C communication. Additionally, the Arduino Nano will not 
  operate stably when powered by a 3.3V supply (provided by the kit).

  The most straightforward and suitable solution, though not very practical, would 
  be to modify the Arduino Nano to operate at 3.3V (by changing the crystal to 8MHz,
  among other modifications, including changing the bootloader). The most viable 
  solution would be to use an "Arduino Pro Mini 3.3V". This way, no changes or 
  adaptations would be necessary to connect the kit to the microcontroller.

  For the Arduino Nano, the adaptation used in this example involves using a 3.3V to 
  5V voltage converter to power the Arduino and placing two 150-ohm resistors in series 
  with the I2C bus. Although this is not best practice, it works to some extent. 
  If you insist in using a Arduino Nano, a more appropriate solution would be to use a 
  bidirectional I2C level shifter to make the I2C communication compatible with both 
  the Arduino Nano and the kit**.

  Finally, the Arduino Nano was used in this example because it is widely used by most 
  hobbyists. It's likely that many already have one at home, or even an Arduino Uno,
  which shares the same architecture.

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  PU7MGR, Grazianny 08 2025.
*/

#include <QN8066.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
// #include "Rotary.h"  // Disabled for Proteus - using polling instead

// LCD 16x2 or LCD20x4 PINs
#define RS 9
#define E  8
#define D4 7
#define D5 6
#define D6 5
#define D7 4

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

#define PWM_PA 10
#define ENCODER_NO_ACTION 0
#define ENCODER_LEFT -1  
#define ENCODER_RIGHT 1
#define BT_MENU       2  

// Enconder PINs
#define ENCODER_PIN_A 3
#define ENCODER_PIN_B 2
#define BT_MENU 12

#define STEP_FREQ 2                // Step Frequencia 200khz
#define PUSH_MIN_DELAY 100
#define STATUS_REFRESH_TIME 1000   // Increased for Proteus visibility (was 350)
#define STOP_RDS_TIME 1000

#define address 0                  // Posição endereço EEPROM determinado

// Encoder polling variables (Proteus compatible)
int encoderCount = 0;              // No longer volatile - not using interrupts
uint8_t lastEncoderState = 0;      // Last encoder state for polling detection
unsigned long lastEncoderTime = 0; // Debounce timing
bool stopRDSforWhile = false;
long stopRDSTime = millis();

long showStatusTime = millis();
uint8_t menuLevel = 0;

// So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPower;              // Power/duty.
uint8_t pwmStep = 1;           // PWM Duty increment and decrement step

// Tables and parameter values based on QN8066 register (see datasheet)
typedef struct
{
uint8_t idx;                       // Value of the parameter (QN8066 register value and description)
const char *desc;                  // Description
} TableValue;

typedef struct
{
  int8_t key;
  TableValue *value;
} KeyValue;

/*
  TRANSMITTER PARAMETERS 
  The following tables represent the possible values allowed for each transmission parameter
  and correspond to the menu item. 
*/

TableValue OffMinuteNoAudio[] = {
  { 0, "Disable" },   // 0 Default
  { 1, "Enable " }    // 1
};

TableValue Impedance[] = {
  { 0, "10K" },   // 0 
  { 1, "20K" },   // 1 Default
  { 2, "40K" },   // 2
  { 3, "80K" }    // 3
};

TableValue GainTxPilot[] = {
  { 7,  "7% " },  // 0
  { 8,  "8% " },  // 1
  { 9,  "9% " },  // 2 Default
  { 10, "10%" }   // 3
};

TableValue SoftClipEnable[] = {
  { 0, "Disable" },  // 0 Default
  { 1, "Enable " }   // 1
};

TableValue SoftClipThreshold[] = {
  { 0, "-3dB  " },  // 0 Default
  { 1, "-4.5dB" },  // 1
  { 2, "-6dB  " },  // 2
  { 3, "-9dB  " }   // 3
};

TableValue FrequencyDeviation[] = {
  { 37,  "25kHz " },  // 0
  { 73,  "50kHz " },  // 1
  { 109, "75kHz " },  // 2 Default
  { 145, "100kHz" },  // 3
  { 181, "125kHz" },  // 4
  { 217, "150kHz" },  // 5
  { 254, "175kHz" }   // 6
};

TableValue BufferGain[] = {
  { 0, "-3dB" },  // 0
  { 1, "0dB " },  // 1 Default
  { 2, "3dB " },  // 2
  { 3, "6dB " },  // 3
  { 4, "9dB " },  // 4
  { 5, "12dB" }   // 5
};

TableValue PreEmphasis[] = {
  { 0, "50us" },  // 0
  { 1, "75us" }   // 1 Default
};

TableValue RDS[] = {
  { 0, "Disable" },  // 0 Default
  { 1, "Enable " }   // 1
};

TableValue RdsPty[]{                    // See https://en.wikipedia.org/wiki/Radio_Data_System if you want to change this table
                        { 0, "None           " },  // 0 Default
                        { 1, "News           " },  // 1
                        { 3, "Information    " },  // 2
                        { 4, "Sport          " },  // 3
                        { 5, "Education      " },  // 4
                        { 7, "Culture        " },  // 5
                        { 8, "Science        " },  // 6
                        { 9, "Varied         " },  // 7
                        { 10, "Pop Music     " },  // 8
                        { 20, "Religion      " },  // 9
                        { 25, "Country Music " },  // 10
                        { 26, "National music" }   // 11
};

TableValue MonoStereo[] = {
  { 0, "Stereo" },  // 0 Default
  { 1, "Mono  " }   // 1 
};


TableValue RdsFreqDev[] = {
  { 3,  "1,0kHz " },  // 0
  { 7,  "2,5kHz " },  // 1
  { 13, "4,5kHz " }   // 2 Default
};

// Menu Itens
const char *menu[] = { "Frequency",
                       "Power",
                       "Stereo/Mono",
                       "Gain Pilot", 
                       "Freq.Dev.",                                             
                       "Pre-Emphasis",
                       "RDS",
                       "RDS Freq.Dev.",                       
                       "RDS PTY",
                       "RDS PS Text",
                       "RDS RT Text",
                       "Impedance",
                       "Buffer Gain. Rin 20K",                       
                       "Soft Clip.Thres.",
                       "Soft Clip",
                       "Off Audio Minute",
                       "Exit" };
int8_t menuIdx = 0;
const int lastMenu = (sizeof(menu) / sizeof(menu[0])) - 1;  // Laste menu item position

// Define the enum with the corresponding Menu Itens QN8066 register values
enum MenuKeys {
  KEY_FREQUENCY,            // 0
  KEY_POWER,                // 1
  KEY_MONO_STEREO,          // 2
  KEY_GAIN_PILOT,           // 3
  KEY_FREQ_DERIVATION,      // 4  
  KEY_PRE_EMPHASIS,         // 5
  KEY_RDS,                  // 6
  KEY_RDS_FREQ_DEV,         // 7  
  KEY_RDS_PTY,              // 8
  KEY_RDS_PS,               // 9
  KEY_RDS_RT,               // 10
  KEY_IMPEDANCE,            // 11
  KEY_BUFFER_GAIN,          // 12  
  KEY_SOFT_CLIP_THRESHOLD,  // 13
  KEY_SOFT_CLIP,            // 14
  KEY_OFF_MINUTE_NOAUDIO,   // 15 
  KEY_EXIT                  // 16
};

/*
  The following table represents an "array" of default values (default position) for each transmitter parameter. 
  Thus, by default, the position ("key") points to the table position that corresponds to the parameter to be 
  configured for the transmitter. Complex? Yes, a bit. But this helps to write less code in C/C++ in case more 
  parameters are added to the system. See KeyValue datatype above.
*/
KeyValue keyValue[] = {
  { 0, NULL },                // KEY_FREQUENCY
  { 0, NULL },                // KEY_POWER
  { 0, MonoStereo },          // KEY_MONO_STEREO
  { 2, GainTxPilot },         // KEY_GAIN_PILOT  
  { 2, FrequencyDeviation },  // KEY_FREQ_DERIVATION  
  { 1, PreEmphasis },         // KEY_PRE_EMPHASIS
  { 0, RDS },                 // KEY_RDS
  { 2, RdsFreqDev },          // KEY_RDS_FREQ_DEV  
  { 0, RdsPty },              // KEY_RDS_PTY
  { 0, NULL },                // KEY_RDS_PS
  { 0, NULL },                // KEY_RDS_RT
  { 2, Impedance },           // KEY_IMPEDANCE
  { 1, BufferGain },          // KEY_BUFFER_GAIN  
  { 0, SoftClipThreshold },   // KEY_SOFT_CLIP_THRESHOLD
  { 0, SoftClipEnable },      // KEY_SOFT_CLIP
  { 0, OffMinuteNoAudio},     // KEY_OFF_MiNUTE_NOAUDIO
  { 0, NULL }                 // KEY_EXIT
};

uint16_t Freq;        

// RDS PS – lista de mensagens (cada uma com 8 chars)
char rdsPSmsg[5][9] = {
  "FM GRAZA",
  "SO AS   ",
  "MELHOR  ",
  "FLASH   ",
  "BACK    "
};
// RDS RT – texto 32 caracteres
char rdsRTmsg[33] = "SO AS MELHORES FLASHBACK     ";

const uint8_t lastRdsPS = (sizeof(rdsPSmsg) / sizeof(rdsPSmsg[0])) - 1;
const uint8_t lastRdsRT = (sizeof(rdsRTmsg) / sizeof(rdsRTmsg[0])) - 1;
uint8_t idxRdsPS = 0;
uint8_t idxRdsRT = 0;

#define RDS_PS_REFRESH_TIME 1000    // padrão(7000)
#define RDS_RT_REFRESH_TIME 1700    // padrão(1700)
#define RDS_DT_REFRESH_TIME 59000   // padrão (59000) Date and Time Service

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

// TX board interface
QN8066 Qn;

// Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);  // Disabled for Proteus

void setup() {
  pinMode(PWM_PA, OUTPUT);           // Sets the Arduino PIN to operate with with PWM
  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);  

// Encoder polling mode for Proteus (interrupts disabled)
  // attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);
  
  // Initialize encoder state
  lastEncoderState = (digitalRead(ENCODER_PIN_A) << 1) | digitalRead(ENCODER_PIN_B);
  lastEncoderTime = millis();

  Qn.setI2CFastMode();
  lcd.begin(20, 4);
  
// If you want to reset the eeprom, keep the BT_MENU button pressed during statup
  if (digitalRead(BT_MENU) == LOW) {
    lcd.clear();
    EEPROM.write(address, 0);
    lcd.setCursor(2, 0);
    lcd.print("RESET TO DEFAULT");
    delay(2000);
    lcd.clear();
  }

  showSplash();
  delay(100);
  lcd.clear();
  
  // Disabled for Proteus simulation (QN8066 model may not be available)
  // checkQN8066();

// Check the EEPROM content. If it contains valid data, read it (previous setup).
  if (EEPROM.read(address) == 10) {
    readEeprom();
  } else {
  // Defult values
  Freq = 981;          // Default frequency is 98.1 MHz
  pwmPower = 128;      // Default PowerRF is 50%
  saveEeprom();
 }

  onPWM(0);  // PWM disable

// Sets the transmitter with the previous setup parameters
  Qn.setup();
  Qn.setTX(Freq);
  delay(100);
  
  Qn.setPAC(24);
  Qn.setTxOffAfterOneMinuteNoAudio(keyValue[KEY_OFF_MINUTE_NOAUDIO].value[keyValue[KEY_OFF_MINUTE_NOAUDIO].key].idx);
  Qn.setTxMono(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].idx);
  Qn.setTxFrequencyDerivation(keyValue[KEY_FREQ_DERIVATION].value[keyValue[KEY_FREQ_DERIVATION].key].idx);
  Qn.setTxPilotGain(keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].idx);
  Qn.setPreEmphasis(keyValue[KEY_PRE_EMPHASIS].value[keyValue[KEY_PRE_EMPHASIS].key].idx);
  Qn.rdsTxEnable(keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx);
  Qn.rdsSetFrequencyDerivation(keyValue[KEY_RDS_FREQ_DEV].value[keyValue[KEY_RDS_FREQ_DEV].key].idx);
  Qn.setTxInputImpedance(keyValue[KEY_IMPEDANCE].value[keyValue[KEY_IMPEDANCE].key].idx);
  Qn.setTxInputBufferGain(keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].idx);
  Qn.setTxSoftClipThreshold(keyValue[KEY_SOFT_CLIP_THRESHOLD].value[keyValue[KEY_SOFT_CLIP_THRESHOLD].key].idx);
  Qn.setTxSoftClippingEnable(keyValue[KEY_SOFT_CLIP].value[keyValue[KEY_SOFT_CLIP].key].idx);
  
  showStatus();
                 // Checking RDS setup
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
    startRDS();
    sendRDS();   // Control the RDS PS and RT messages with this function
 }
  onPWM(pwmPower);  // It is about 1/5 of the max power. At 50 duty cycle, it is between 1 and 1,4 W
}

/**
 * Reads encoder via POLLING (Proteus compatible)
 * This function replaces interrupt-based encoder reading
 * Call this frequently in loop() or before checking encoder state
 */
void readEncoderPolling() {
  // Debounce check - only read every 5ms
  if (millis() - lastEncoderTime < 5) return;
  
  // Read current encoder state
  uint8_t a = digitalRead(ENCODER_PIN_A);
  uint8_t b = digitalRead(ENCODER_PIN_B);
  uint8_t currentState = (a << 1) | b;
  
  // Detect state change
  if (currentState != lastEncoderState) {
    // Simple quadrature decoding
    // State transitions: 00 -> 01 -> 11 -> 10 -> 00 (CW)
    //                   00 -> 10 -> 11 -> 01 -> 00 (CCW)
    
    if (lastEncoderState == 0b00) {
      if (currentState == 0b01) encoderCount = -1;      // CCW
      else if (currentState == 0b10) encoderCount = 1;  // CW
    }
    else if (lastEncoderState == 0b01) {
      if (currentState == 0b11) encoderCount = -1;      // CCW
      else if (currentState == 0b00) encoderCount = 1;  // CW
    }
    else if (lastEncoderState == 0b11) {
      if (currentState == 0b10) encoderCount = -1;      // CCW
      else if (currentState == 0b01) encoderCount = 1;  // CW
    }
    else if (lastEncoderState == 0b10) {
      if (currentState == 0b00) encoderCount = -1;      // CCW
      else if (currentState == 0b11) encoderCount = 1;  // CW
    }
    
    lastEncoderState = currentState;
    lastEncoderTime = millis();
  }
}

void checkQN8066() {
  if (!Qn.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1);
  }
}

void startRDS() {
  uint8_t ptyIdx = keyValue[KEY_RDS_PTY].value[keyValue[KEY_RDS_PTY].key].idx;
  Qn.rdsInitTx(0x8, 0x1, 0x9B, ptyIdx, 50, 8);     // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
}
 
// The update function/method writes data only if the current data is not equal to the stored data.
void saveEeprom() {      
  EEPROM.update(address, 10);
  EEPROM.update(address + 1, Freq >> 8);        // stores the current Frequency HIGH byte for the band
  EEPROM.update(address + 2, Freq & 0xFF);      // stores the current Frequency LOW byte for the band
  EEPROM.update(address + 3, pwmPower);
  EEPROM.update(address + 4, keyValue[KEY_MONO_STEREO].key);
  EEPROM.update(address + 5, keyValue[KEY_PRE_EMPHASIS].key);
  EEPROM.update(address + 6, keyValue[KEY_RDS].key);
  EEPROM.update(address + 7, keyValue[KEY_IMPEDANCE].key);
  EEPROM.update(address + 8, keyValue[KEY_SOFT_CLIP].key);
  EEPROM.update(address + 9, keyValue[KEY_SOFT_CLIP_THRESHOLD].key);
  EEPROM.update(address + 10, keyValue[KEY_GAIN_PILOT].key);
  EEPROM.update(address + 11, keyValue[KEY_FREQ_DERIVATION].key);
  EEPROM.update(address + 12, keyValue[KEY_BUFFER_GAIN].key);
  EEPROM.update(address + 13, keyValue[KEY_RDS_FREQ_DEV].key);
  EEPROM.update(address + 14, keyValue[KEY_RDS_PTY].key);
  EEPROM.update(address + 15, keyValue[KEY_OFF_MINUTE_NOAUDIO].key);
  // ----- SALVAR RDS PS (5 blocos de 8) -----
  for (int linha = 0; linha < 5; linha++) {
     for (int i = 0; i < 8; i++) {
        EEPROM.update(address + 16 + (linha * 8) + i, rdsPSmsg[linha][i]);
    }
  }
  // ----- SALVAR RDS RT (32 chars) -----
  for (int i = 0; i < 32; i++) {
     EEPROM.update(address + 56 + i, rdsRTmsg[i]);
  }
}
    
// Read the previous transmitter setup     
void readEeprom() {       
  Freq = EEPROM.read(address + 1) << 8;
  Freq |= EEPROM.read(address + 2);
  pwmPower = EEPROM.read(address + 3);
  keyValue[KEY_MONO_STEREO].key = EEPROM.read(address + 4);
  keyValue[KEY_PRE_EMPHASIS].key = EEPROM.read(address + 5);
  keyValue[KEY_RDS].key = EEPROM.read(address + 6);
  keyValue[KEY_IMPEDANCE].key = EEPROM.read(address + 7);
  keyValue[KEY_SOFT_CLIP].key = EEPROM.read(address + 8);
  keyValue[KEY_SOFT_CLIP_THRESHOLD].key = EEPROM.read(address + 9);
  keyValue[KEY_GAIN_PILOT].key = EEPROM.read(address + 10);
  keyValue[KEY_FREQ_DERIVATION].key = EEPROM.read(address + 11);
  keyValue[KEY_BUFFER_GAIN].key = EEPROM.read(address + 12);
  keyValue[KEY_RDS_FREQ_DEV].key = EEPROM.read(address + 13);
  keyValue[KEY_RDS_PTY].key = EEPROM.read(address + 14);
  keyValue[KEY_OFF_MINUTE_NOAUDIO].key = EEPROM.read(address + 15);
  // ----- LER PS -----
  for (int linha = 0; linha < 5; linha++) {
     for (int i = 0; i < 8; i++) {
        rdsPSmsg[linha][i] = EEPROM.read(address + 16 + (linha * 8) + i);
    }
    rdsPSmsg[linha][8] = '\0';   // final string
  }
  // ----- LER RT -----
  for (int i = 0; i < 32; i++) {
     rdsRTmsg[i] = EEPROM.read(address + 56 + i);
  }
  rdsRTmsg[32] = '\0';
}

// Enable or disable PWM duty cycle
void onPWM(uint8_t value) {
  pinMode(PWM_PA, OUTPUT); 
  // Simplified for Proteus - direct PWM without ramp
  analogWrite(PWM_PA, value);
}
       
// Switches the the current frequency to a new frequency
void switchTxFrequency(uint16_t freqSw) {
  Qn.setTX(Freq = freqSw);
  showFrequency();
}
    
// Shows the first message after turn the transmitter On
void showSplash() {
  lcd.setCursor(5, 0);
  lcd.print("FM GRAZA");
  delay(100);
  lcd.setCursor(4, 2);
  lcd.print("PLL QN8066");
  lcd.display();
  delay(100);
}

// Editor PS
void editaPS() {
  uint8_t linha = 0;      // 0..3
  uint8_t pos = 0;        // 0..7
  char lastChar;
  static unsigned long lastAction = 0;

  lcd.clear();
  for (uint8_t l = 0; l < 4; l++) {
     lcd.setCursor(0, l);
     lcd.print(rdsPSmsg[l]);
  }
   
  while (true) {
    // Cursor fixo e estável
    lcd.setCursor(pos, linha);
    lcd.cursor();
    lastChar = rdsPSmsg[linha][pos];

    // Read encoder using polling
    readEncoderPolling();
    
    // Debounce for character changes
    if (millis() - lastAction > 150) {
      // ----------- UP (encoder RIGHT) ---------------
      if (encoderCount > 0) {
        rdsPSmsg[linha][pos]++;
        if (rdsPSmsg[linha][pos] > 0x7F)
          rdsPSmsg[linha][pos] = ' ';
        encoderCount = 0;
        lastAction = millis();
      }

      // ---------- DOWN (encoder LEFT) --------------
      if (encoderCount < 0) {
        rdsPSmsg[linha][pos]--;
        if (rdsPSmsg[linha][pos] < ' ')
          rdsPSmsg[linha][pos] = 0x7F;
        encoderCount = 0;
        lastAction = millis();
      }
    }

    // Atualiza somente esse caractere
    if (rdsPSmsg[linha][pos] != lastChar) {
      lcd.setCursor(pos, linha);
      lcd.print(rdsPSmsg[linha][pos]);
    }

    // ----------- SET --------------
    if (digitalRead(BT_MENU) == LOW) {
      unsigned long t0 = millis();
      while (digitalRead(BT_MENU) == LOW) {
        readEncoderPolling();  // Keep polling while button pressed
      }

        // -------- Long press: troca de linha --------
        if (millis() - t0 > 500) {
          linha++;
          pos = 0;
          if (linha > 3) {   // fim da edição
            lcd.noCursor();
            return;
          }
          } else {
            // -------- Press curto: avança caractere --------
            pos++;
            // *** AQUI ESTÁ A CORREÇÃO ***
            if (pos >= 8) {
              pos = 0;
              linha++;
              if (linha > 3) {
                lcd.noCursor();
                return;
              }
            }
         }
         delay(150);
    }
    delay(10);
  }
  lcd.noCursor();
}

// Editor de RDS RT (32 caracteres) 
void editaRT() {
   uint8_t pos = 0;           // posição 0..31
   char lastChar;
   static unsigned long lastAction = 0;

   lcd.clear();
   // Mostra RT dividido em 20 + 12 caracteres
   lcd.setCursor(0, 0);
   for (uint8_t i = 0; i < 20; i++)
      lcd.print(rdsRTmsg[i]);
   lcd.setCursor(0, 1);
   for (uint8_t i = 20; i < 32; i++)
      lcd.print(rdsRTmsg[i]);
   while (true) {
     uint8_t linha = (pos < 20) ? 0 : 1;
     uint8_t coluna = (pos < 20) ? pos : pos - 20;
     // Cursor fixo e estável
     lcd.setCursor(coluna, linha);
     lcd.cursor();
     // Guarda caractere antes de mudar
     lastChar = rdsRTmsg[pos];

     // Read encoder using polling
     readEncoderPolling();
     
     // Debounce for character changes
     if (millis() - lastAction > 150) {
       // ------------- UP (encoder RIGHT) --------------
       if (encoderCount > 0) {
         rdsRTmsg[pos]++;
         if (rdsRTmsg[pos] > 0x7F)
           rdsRTmsg[pos] = ' ';
         encoderCount = 0;
         lastAction = millis();
       }

       // ------------ DOWN (encoder LEFT) -------------
       if (encoderCount < 0) {
         rdsRTmsg[pos]--;
         if (rdsRTmsg[pos] < ' ')
           rdsRTmsg[pos] = 0x7F;
         encoderCount = 0;
         lastAction = millis();
       }
     }

     // -------- Atualiza somente se mudou --------
     if (rdsRTmsg[pos] != lastChar) {
       lcd.setCursor(coluna, linha);
       lcd.print(rdsRTmsg[pos]);
     }
     
     // ------------- SET --------------
     if (digitalRead(BT_MENU) == LOW) {
       unsigned long t0 = millis();
       while (digitalRead(BT_MENU) == LOW) {
         readEncoderPolling();  // Keep polling while button pressed
       }
       // Long press (só se quiser voltar ao menu)
       if (millis() - t0 > 500) {
         lcd.noCursor();
         return;    // sai da edição completamente
       }
       // Press curto → avança caractere
       pos++;

       // ------------- AUTO MUDANÇA DE LINHA ---------------
       if (pos == 20) {
         linha = 1;
         coluna = 0;
       }

       // ------------- FIM DOS 32 CARACTERES ---------------
       if (pos >= 32) {
         lcd.noCursor();
         return;
       }
       delay(120);
     }
     delay(10);
   }
   lcd.noCursor();
}

// Show the current frequency Menu
void showFrequency() {
  char strFrequency[7];
  Qn.convertToChar(Freq, strFrequency, 4, 3, '.');  // Convert the selected frequency a array of char
  lcd.setCursor(1, 1);
  lcd.print(strFrequency);
  lcd.print("MHz");
  lcd.display();
}
    
// Shows the current power in percent (duty cycle) Menu
void showPower() {
  char strPower[12];
  uint16_t currentPower = (uint16_t)(pwmPower * 100 / 255);
  sprintf(strPower, "%d%%  ", currentPower);
  lcd.setCursor(1, 1);
  lcd.print(strPower);
}

// Shows the general current transmitter status LCD
void showStatus() {
  char strFrequency[9];
  char str[20];
  //checkQN8066();
  Qn.resetAudioPeak();
    
// Frequency 
  Qn.convertToChar(Freq, strFrequency, 4, 3, '.');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 0);
  lcd.print(strFrequency);
  lcd.print("MHz");

// Modo  
  lcd.setCursor(14, 0);
  lcd.print(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].desc);  // Mono/Stereo
     
// Audio Peak
  int pk = Qn.getAudioPeakValue();
  //lcd.setCursor(8, 1);
  //lcd.print(pk);
  //lcd.print("  ");
  lcd.setCursor(0, 2);
  lcd.print("Pk:");
  if (pk == 675) {
    lcd.setCursor(3, 2);
    lcd.print("Clip");
  } else {
    lcd.print("Ok  ");
}

// Power 
  lcd.setCursor(13, 1);
  lcd.print("Pa:");
  lcd.print(pwmPower * 100 / 255);           
  lcd.print("%");
        
// RDS
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
    sprintf(str, "PS:%s   PTY:%2d", Qn.rdsGetPS(), Qn.rdsGetPTY());
    lcd.setCursor(0, 3);
    lcd.print(str);
    lcd.setCursor(13, 2);
    sprintf(str, "Err: %d", Qn.rdsGetError());
    lcd.print(str);
  } else {     
     lcd.setCursor(13, 2);
     lcd.print("Rds:Off");
  }       
      
// Fsm Status
  int Status = Qn.getFsmStateCode();
  lcd.setCursor(0, 1);
  if (Status == 10)
    lcd.print("Lock  ");
  else if (Status == 12)
    lcd.print("No Tx ");
  else  
    lcd.print("Unlock");
  lcd.display();
}     

// Shows the given parameter to be updated
void showParameter(char *desc) {
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(desc);
  lcd.display();
}
     
int8_t browseParameter() {
  int8_t browse;
  do {
    browse = checkEncoder();
  if (browse == ENCODER_LEFT)  // Down/Left pressed
    return ENCODER_LEFT;
  else if (browse == ENCODER_RIGHT)  // Up/Right pressed
    return ENCODER_RIGHT;
  } while (browse == ENCODER_NO_ACTION);
    return BT_MENU;
}
     
// Shows current menu data
void showMenu(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menu[idx]);
  if (keyValue[idx].value != NULL) {
    lcd.setCursor(0, 1);
    lcd.print(keyValue[idx].value[keyValue[idx].key].desc);
  } else if (idx == 0) {
    showFrequency();
  } else if (idx == 1) {
    showPower();
  }
}
     
// Processes the change to a new frequency
void doFrequency() {
  showFrequency();
  int8_t key = browseParameter();
  while (key != BT_MENU) {
    if (key == -1) {
      if (Freq <= 761)   // If less than 76.1 MHz
        Freq = 1079;
      else
        Freq -= STEP_FREQ;
    } else if (key == 1) {
    if (Freq >= 1079)  // if more than 107.9 MHz
      Freq = 761;
    else
      Freq += STEP_FREQ;
    }
    switchTxFrequency(Freq);
    showFrequency();
    key = browseParameter();
  }
  menuLevel = 0;
}
      
// Processes the change to a new power (PWM duty cycle)
void doPower() {
  showPower();
  int8_t key = browseParameter();
  while (key != BT_MENU) {
    if (key == -1) {
      if (pwmPower >= 25)
        pwmPower -= pwmStep;
      else
        pwmPower = 0;
    } else if (key == 1) {
             if (pwmPower <= 225)
               pwmPower += pwmStep;
             else
               pwmPower = 255;
    }
    pinMode(PWM_PA, OUTPUT); 
    analogWrite(PWM_PA, pwmPower);
    showPower();
    key = browseParameter();
  }
  menuLevel = 0;
}
/**
 * @brief Runs the action menu to modify the given parameter.
 * @details This function generalizes the menu functions that modify the transmitter parameter values.
 * @details For this purpose, it receives as parameters pointers to functions that will be executed (depending on what needs to be modified).
 * @param showFunc    Pointer to the function that displays the parameter.
 * @param actionFunc  Pointer to the function that modifies the parameter.
 * @param tab         Table (see KeyValue) that contains the set of valid parameters.
 * @param step        Step (increment) used for the parameter.
 * @param min         Minimum valid value.
 * @param max         Maximum valid value.
 * @see C/C++: Syntax for declaring function pointers; Assigning functions to function pointers; Calling functions through function pointers.
 * @see C/C++: Passing function pointers as arguments to other functions; Understanding how to use function pointers for callback mechanisms.
 * @see   C++: Capturing variables in lambdas and their usage as function pointers; Understanding lambda expressions in C++ and how they relate to function pointers  
 */
void runAction(void (*actionFunc)(uint8_t), KeyValue *tab, uint8_t step, uint8_t min, uint8_t max) {
  showParameter((char *)tab->value[tab->key].desc);
  int8_t key = browseParameter();
   while (key != BT_MENU) {
     if (key == ENCODER_RIGHT) {
       if (tab->key == max)
         tab->key = min;
       else
         tab->key = tab->key + step;
     } else {
       if (tab->key == min)
         tab->key = max;
       else
         tab->key = tab->key - step;
     }
    actionFunc(tab->value[tab->key].idx);
    showParameter((char *)tab->value[tab->key].desc);
    key = browseParameter();
  }
  // menuLevel = 0;
}

// Processes the current menu option selected
uint8_t doMenu(uint8_t idxMenu) {
  delay(PUSH_MIN_DELAY);
  switch (idxMenu) {
    case KEY_FREQUENCY:
      onPWM(0);              // The PWM off interfere with the communication with the QN8066.
      lcd.setCursor(0, 1);
      lcd.print(">");        // it just indicates the edit mode
      doFrequency();
      onPWM(pwmPower);   // Turn the PWM on again.
      break;
    
    case KEY_POWER:
      lcd.setCursor(0, 1);
      lcd.print(">");        // it just indicates the edit mode
      doPower();
      break;
    
    case KEY_MONO_STEREO:
      runAction([](uint8_t value) {
        Qn.setTxMono(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    
    case KEY_PRE_EMPHASIS:
      runAction([](uint8_t value) {
        Qn.setPreEmphasis(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    
    case KEY_RDS:
      runAction([](uint8_t value) {
        Qn.rdsTxEnable(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      startRDS();
      break;
    
    case KEY_RDS_PTY:
      runAction([](uint8_t value) {
        Qn.rdsSetPTY(value);
      },
                &keyValue[idxMenu], 1, 0, 11);
      break;
    
    case KEY_RDS_PS:
      editaPS();
      break;
    
    case KEY_RDS_RT:
      editaRT(); 
      break; 
    
    case KEY_IMPEDANCE:
      runAction([](uint8_t value) {
        Qn.setTxInputImpedance(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    
    case KEY_SOFT_CLIP:
      runAction([](uint8_t value) {
        Qn.setTxSoftClippingEnable(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    
    case KEY_SOFT_CLIP_THRESHOLD:
      runAction([](uint8_t value) {
        Qn.setTxSoftClipThreshold(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    
    case KEY_GAIN_PILOT:
      runAction([](uint8_t value) {
        Qn.setTxPilotGain(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    
    case KEY_FREQ_DERIVATION:
      runAction([](uint8_t value) {
        Qn.setTxFrequencyDerivation(value);
      },
                &keyValue[idxMenu], 1, 0, 6);
      break;
    
    case KEY_BUFFER_GAIN:
      runAction([](uint8_t value) {
        Qn.setTxInputBufferGain(value);
      },
                &keyValue[idxMenu], 1, 0, 5);
      break;
    
    case KEY_RDS_FREQ_DEV:
      runAction([](uint8_t value) {
        Qn.rdsSetFrequencyDerivation(value);
      },
                &keyValue[idxMenu], 1, 0, 2);
      break;
    
    case KEY_OFF_MINUTE_NOAUDIO:
      runAction([](uint8_t value) {
        Qn.setTxOffAfterOneMinuteNoAudio(value);
      },
                &keyValue[idxMenu], 1, 0, 2);
      break;
    
    case KEY_EXIT:
      return 0;
      break;
    default:
      break;
  }
  saveEeprom();  // Saves the current modified data to the EEPROM
  return 1;
}

void sendRDS() {
// PS refreshing control
  if ((millis() - rdsTimePS) > RDS_PS_REFRESH_TIME) {
    if (idxRdsPS > lastRdsPS) idxRdsPS = 0;
    delay(10);   // padrão (100)
    Qn.rdsSendPS(rdsPSmsg[idxRdsPS]);
    idxRdsPS++;
    rdsTimePS = millis();
  }

// RT refreshing control
  if ((millis() - rdsTimeRT) > RDS_RT_REFRESH_TIME) {
    if (idxRdsRT > lastRdsRT) idxRdsRT = 0;
    delay(10);    // padrão(100)
    Qn.rdsSendRT(rdsRTmsg);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    idxRdsRT++;
    rdsTimeRT = millis();
  }

// Date Time Service refreshing control
  if ((millis() - rdsDateTime) > RDS_DT_REFRESH_TIME) {
    delay(100);
    // To use the function (service) below, you will need to add an integrated clock to your 
    // system that provides the date and time to the system. The following example presents 
    // only a fixed date and time and is intended solely to illustrate the use of the function.
    Qn.rdsSendDateTime(2025, 8, 19, 9, 15, 0);  // Sends Year = 2025; month = 8; day = 18; At 15:01 (local time)    
    rdsDateTime = millis();
  }
}

int8_t checkEncoder() {
  static unsigned long lastDebounce = 0;
  const unsigned long debounceDelay = 50;
  int8_t action = ENCODER_NO_ACTION;
  
  // Read encoder state via polling
  readEncoderPolling();
  
  // Check for debounce timeout
  if (millis() - lastDebounce < debounceDelay) {
    return ENCODER_NO_ACTION;
  }
  
  // Check menu button first (highest priority)
  if (digitalRead(BT_MENU) == LOW) {
    while (digitalRead(BT_MENU) == LOW) {
      readEncoderPolling();  // Keep polling while waiting
    }
    lastDebounce = millis();
    return BT_MENU;
  }
  
  // Check encoder count from polling
  if (encoderCount > 0) {
    action = ENCODER_RIGHT;
    encoderCount = 0;
    lastDebounce = millis();
  }
  else if (encoderCount < 0) {
    action = ENCODER_LEFT;
    encoderCount = 0;
    lastDebounce = millis();
  }
  
  return action;
}

void loop() {
  int8_t key;
  
  // Always poll encoder (critical for Proteus)
  readEncoderPolling();
  
  if (menuLevel == 0) {
    showStatus();
    while ((key = checkEncoder()) == ENCODER_NO_ACTION) {
      if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1  && !stopRDSforWhile ) {
        sendRDS();
      }

      if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1 && stopRDSforWhile && (millis() - stopRDSTime) > STOP_RDS_TIME ) { 
        stopRDSforWhile = false; 
        Qn.rdsTxEnable(true);
        stopRDSTime = millis();
      }

// Refresh Status
      if ((millis() - showStatusTime) > STATUS_REFRESH_TIME) {
        showStatus();
        showStatusTime = millis();
      }
    }
// If you RDS is configured, stop for a while the RDS processing to improve the encoder and buttons response. 
    stopRDSforWhile = true;
    if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) Qn.rdsTxEnable(false);
      if ( key == BT_MENU) {    // Menu Pressed
      menuLevel = 1;
    } 
    } else if (menuLevel == 1) {
     showMenu(menuIdx);
     key = browseParameter();
      while (key != BT_MENU) {
        if (key == ENCODER_LEFT) {
          if (menuIdx == 0)
            menuIdx = lastMenu;
        else
            menuIdx--;
        } else if (key == ENCODER_RIGHT) {
          if (menuIdx == lastMenu)
            menuIdx = 0;
        else
            menuIdx++;
        }
        showMenu(menuIdx);
        key = browseParameter();
      }
  menuLevel = 2;
  } else if (menuLevel == 2) {
    menuLevel = doMenu(menuIdx);
  }

  stopRDSTime = millis();           // Back RDS only on main manu / main screen
  delay(5);
}
//End