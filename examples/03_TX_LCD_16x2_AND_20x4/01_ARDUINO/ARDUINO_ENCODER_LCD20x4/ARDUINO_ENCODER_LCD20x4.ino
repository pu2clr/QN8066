/*
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD16X02.

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
  |                           | D4                        |     D4        |
  |                           | D5                        |     D5        |
  |                           | D6                        |     D6        |
  |                           | D7                        |     D7        |
  |                           | RS                        |     D2       |
  |                           | E/ENA                     |     D3       |
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
  |                           | Menu                      |     8         |
  |                           | Left (Down / -)           |     11        |
  |                           | Right (Up / + )           |     10        |
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

  By PU2CLR, Ricardo,  Feb  2023.
*/

#include <QN8066.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "Rotary.h"

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

// Enconder PINs
#define BT_MENU 8

#define PWM_PA 9


#define ENCODER_NO_ACTION 0
#define ENCODER_LEFT -1  
#define ENCODER_RIGHT 1
#define BT_MENU_PRESSED 2  

// Enconder PINs
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3


#define STEP_FREQ 1
#define PUSH_MIN_DELAY 100
#define STATUS_REFRESH_TIME 4000
#define STOP_RDS_TIME 10000

volatile int encoderCount = 0;
bool stopRDSforWhile = false;
long stopRDSTime = millis();

long showStatusTime = millis();
uint8_t menuLevel = 0;
const uint8_t app_id = 80;          // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;

      // So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPowerDuty = 50;  // Initial power/duty.
uint8_t pwmDutyStep = 5;    // PWM Duty increment and decrement step

     // Tables and parameter values based on QN8066 register (see datasheet)
typedef struct
{
uint8_t idx;       // Value of the parameter (QN8066 register value and description)
const char *desc;  // Description
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

TableValue tabImpedance[] = {
  { 0, "10K" },   // 0
  { 1, "20K" },   // 1
  { 2, "40K" },   // 2
  { 3, "80K" }    // 3
};

TableValue tabGainTxPilot[] = {
  { 7,  "7% " },  // 0
  { 8,  "8% " },  // 1
  { 9,  "9% " },  // 2
  { 10, "10%" }   // 3
};

TableValue tabTxSoftClipEnable[] = {
  { 0, "Disable" },  // 0
  { 1, "Enable " }   // 1
};

TableValue tabTxSoftClipThreshold[] = {
  { 0, "3dB  " },  // 0
  { 1, "4.5dB" },  // 1
  { 2, "6dB  " },  // 2
  { 3, "9dB  " }   // 3
};

TableValue tabTxFrequencyDeviation[] = {
  { 60,  "40kHz " },  // 0
  { 87,  "60kHz " },  // 1
  { 108, "75kHz " },  // 2
  { 120, "92kHz " },  // 3
  { 140, "96kHz " },  // 4
  { 160, "110kHz" }   // 5
};

TableValue tabTxBufferGain[] = {
  { 0, "3dB " },  // 0
  { 1, "6dB " },  // 1
  { 2, "9dB " },  // 2
  { 3, "12dB" },  // 3
  { 4, "15dB" },  // 4
  { 5, "18dB" }   // 5
};

TableValue tabPreEmphasis[] = {
  { 0, "50us" },  // 0
  { 1, "75us" }   // 1
};

TableValue tabRDS[] = {
  { 0, "Disable" },  // 0
  { 1, "Enable " }   // 1
};

TableValue tabRdsPty[]{                    // See https://en.wikipedia.org/wiki/Radio_Data_System if you want to change this table
                        { 0, "None           " },  // 0
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

TableValue tabMonoStereo[] = {
  { 0, "Stereo" },  // 0
  { 1, "Mono  " }   // 1 - See QN8066 data sheet
};


TableValue txRdsFreqDev[] = {
  { 3,  "1,0kHz " },  // 0
  { 7,  "2,5kHz " },  // 1
  { 13, "4,5kHz " },  // 2
  { 26, "9,0kHz " },  // 3
  { 39, "13,5kHz" },  // 4
  { 52, "18,0kHz" }   // 5
};

     // Menu Itens
const char *menu[] = { "Frequency",
                       "Power",
                       "Stereo/Mono",
                       "Gain Pilot", 
                       "Freq.Deriv.",                                             
                       "Pre-emphasis",
                       "RDS",
                       "RDS Freq.Dev.",                       
                       "RDS PTY",
                       "Impedance",
                       "Buffer gain",                       
                       "Soft Clip.Thres.",
                       "Soft Clip",
                       "EXIT" };
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
  KEY_IMPEDANCE,            // 9
  KEY_BUFFER_GAIN,          // 10  
  KEY_SOFT_CLIP_THRESHOLD,  // 11
  KEY_SOFT_CLIP,            // 12
  KEY_EXIT                  // 13
};

/*
  The following table represents an "array" of default values (default position) for each transmitter parameter. 
  Thus, by default, the position ("key") points to the table position that corresponds to the parameter to be 
  configured for the transmitter. Complex? Yes, a bit. But this helps to write less code in C/C++ in case more 
  parameters are added to the system. See KeyValue datatype above.
*/
KeyValue keyValue[] = {
  { 0, NULL },                     // KEY_FREQUENCY
  { 0, NULL },                     // KEY_POWER
  { 0, tabMonoStereo },            // KEY_MONO_STEREO
  { 2, tabGainTxPilot },           // KEY_GAIN_PILOT  
  { 2, tabTxFrequencyDeviation },  // KEY_FREQ_DERIVATION  
  { 1, tabPreEmphasis },           // KEY_PRE_EMPHASIS
  { 0, tabRDS },                   // KEY_RDS
  { 2, txRdsFreqDev },             // KEY_RDS_FREQ_DEV  
  { 0, tabRdsPty },                // KEY_RDS_PTY
  { 2, tabImpedance },             // KEY_IMPEDANCE
  { 1, tabTxBufferGain },          // KEY_BUFFER_GAIN  
  { 0, tabTxSoftClipThreshold },   // KEY_SOFT_CLIP_THRESHOLD
  { 0, tabTxSoftClipEnable },      // KEY_SOFT_CLIP
  { 0, NULL }                      // KEY_EXIT
};

uint16_t txFrequency = 1069;      // Default frequency is 106.9 MHz
     
      // Station Name (PS) messages
char *rdsPSmsg[] = { (char *)"FM GRA \r",
                     (char *)"SO AS  \r",
                     (char *)"MELHOR \r",
                     (char *)"FLASH  \r",
                     (char *)"BACK   \r" };

     // Radio Text (RT) messages
     // Apparently, some receivers seem to work better when the special character 0xD (\r)
     // is added at the end of the string (message). Therefore, although it is technically
     // possible to work with messages up to 64 characters long, it is recommended to keep
     // the message length within 32 characters by default, reserving the 32nd character for
     // the '\r' as shown below.
char *rdsRTmsg[] = { (char *)"SO AS MELHORES FLASHBACK       \r" };

const uint8_t lastRdsPS = (sizeof(rdsPSmsg) / sizeof(rdsPSmsg[0])) - 1;
const uint8_t lastRdsRT = (sizeof(rdsRTmsg) / sizeof(rdsRTmsg[0])) - 1;
uint8_t idxRdsPS = 0;
uint8_t idxRdsRT = 0;

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();


long  timeInTheAr = millis();
long  countI2CError = 0;  
long  countTime = 0;


      // TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);


void setup() {
  
  pinMode(PWM_PA, OUTPUT);  // Sets the Arduino PIN to operate with with PWM
  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);  


  // controlling encoder via interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);


  tx.setI2CFastMode();
  lcd.begin(20, 4);

      // If you want to reset the eeprom, keep the BT_MENU button pressed during statup
    if (digitalRead(BT_MENU) == LOW) {
    lcd.clear();
    EEPROM.write(eeprom_address, 0);
    lcd.setCursor(2, 0);
    lcd.print("RESET TO DEFAULT");
    delay(2000);
    lcd.clear();
  }

showSplash();
  delay(2000);
  lcd.clear();
  //checkQN8066();

        // Check the EEPROM content. If it contains valid data, read it (previous setup).
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllTransmitterInformation();
  } else {
        // Defult values
    txFrequency = 1069;
    pwmPowerDuty = 128;
    saveAllTransmitterInformation();
  }

  enablePWM(0);  // PWM disable

                 // Sets the transmitter with the previous setup parameters
  tx.setup();
  tx.setTX(txFrequency);
  delay(100);
  tx.setTxMono(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].idx);
  tx.setTxFrequencyDerivation(keyValue[KEY_FREQ_DERIVATION].value[keyValue[KEY_FREQ_DERIVATION].key].idx);
  tx.setTxPilotGain(keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].idx);
  tx.setPreEmphasis(keyValue[KEY_PRE_EMPHASIS].value[keyValue[KEY_PRE_EMPHASIS].key].idx);
  tx.rdsTxEnable(keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx);
  tx.rdsSetFrequencyDerivation(keyValue[KEY_RDS_FREQ_DEV].value[keyValue[KEY_RDS_FREQ_DEV].key].idx);
  tx.setTxInputImpedance(keyValue[KEY_IMPEDANCE].value[keyValue[KEY_IMPEDANCE].key].idx);
  tx.setTxInputBufferGain(keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].idx);
  tx.setTxSoftClipThreshold(keyValue[KEY_SOFT_CLIP_THRESHOLD].value[keyValue[KEY_SOFT_CLIP_THRESHOLD].key].idx);
  tx.setTxSoftClippingEnable(keyValue[KEY_SOFT_CLIP].value[keyValue[KEY_SOFT_CLIP].key].idx);

showStatus();
                 // Checking RDS setup
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
    startRDS();
    sendRDS();   // Control the RDS PS and RT messages with this function
  }

  enablePWM(pwmPowerDuty);  // It is about 1/5 of the max power. At 50 duty cycle, it is between 1 and 1,4 W
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



void checkQN8066() {
  if (!tx.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1);
  }
}

void startRDS() {
    uint8_t ptyIdx = keyValue[KEY_RDS_PTY].value[keyValue[KEY_RDS_PTY].key].idx;
    tx.rdsInitTx(0x8,0x1,0x9B, ptyIdx, 50, 8);     // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
}
 
    // The update function/method writes data only if the current data is not equal to the stored data.
void saveAllTransmitterInformation() {      
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, txFrequency >> 8);        // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 2, txFrequency & 0xFF);      // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 3, pwmPowerDuty);
  EEPROM.update(eeprom_address + 4, keyValue[KEY_MONO_STEREO].key);
  EEPROM.update(eeprom_address + 5, keyValue[KEY_PRE_EMPHASIS].key);
  EEPROM.update(eeprom_address + 6, keyValue[KEY_RDS].key);
  EEPROM.update(eeprom_address + 7, keyValue[KEY_IMPEDANCE].key);
  EEPROM.update(eeprom_address + 8, keyValue[KEY_SOFT_CLIP].key);
  EEPROM.update(eeprom_address + 9, keyValue[KEY_SOFT_CLIP_THRESHOLD].key);
  EEPROM.update(eeprom_address + 10, keyValue[KEY_GAIN_PILOT].key);
  EEPROM.update(eeprom_address + 11, keyValue[KEY_FREQ_DERIVATION].key);
  EEPROM.update(eeprom_address + 12, keyValue[KEY_BUFFER_GAIN].key);
  EEPROM.update(eeprom_address + 13, keyValue[KEY_RDS_FREQ_DEV].key);
  EEPROM.update(eeprom_address + 14, keyValue[KEY_RDS_PTY].key);
}
    
    // Read the previous transmitter setup     
void readAllTransmitterInformation() {       
  txFrequency = EEPROM.read(eeprom_address + 1) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 2);
  pwmPowerDuty = EEPROM.read(eeprom_address + 3);
  keyValue[KEY_MONO_STEREO].key = EEPROM.read(eeprom_address + 4);
  keyValue[KEY_PRE_EMPHASIS].key = EEPROM.read(eeprom_address + 5);
  keyValue[KEY_RDS].key = EEPROM.read(eeprom_address + 6);
  keyValue[KEY_IMPEDANCE].key = EEPROM.read(eeprom_address + 7);
  keyValue[KEY_SOFT_CLIP].key = EEPROM.read(eeprom_address + 8);
  keyValue[KEY_SOFT_CLIP_THRESHOLD].key = EEPROM.read(eeprom_address + 9);
  keyValue[KEY_GAIN_PILOT].key = EEPROM.read(eeprom_address + 10);
  keyValue[KEY_FREQ_DERIVATION].key = EEPROM.read(eeprom_address + 11);
  keyValue[KEY_BUFFER_GAIN].key = EEPROM.read(eeprom_address + 12);
  keyValue[KEY_RDS_FREQ_DEV].key = EEPROM.read(eeprom_address + 13);
  keyValue[KEY_RDS_PTY].key = EEPROM.read(eeprom_address + 14);
}

    // Enable or disable PWM duty cycle
void enablePWM(uint8_t value) {
  pinMode(PWM_PA, OUTPUT); 
  if (value == 0) {
    analogWrite(PWM_PA, value);
    return;
  }
  for (uint8_t i = 0; i < value; i++) {
    analogWrite(PWM_PA, i);
    delay(5);
  }
}
       
    // Switches the the current frequency to a new frequency
void switchTxFrequency(uint16_t freq) {
  tx.setTX(txFrequency = freq);
  showFrequency();
}
    
    // Shows the first message after turn the transmitter on
void showSplash() {
  lcd.setCursor(5, 0);
  lcd.print("FM GRAZA");
  delay(1000);
  lcd.setCursor(4, 2);
  lcd.print("PLL QN8066");
  lcd.display();
  delay(500);
}
    
    // Show the current frequency
void showFrequency() {
  char strFrequency[7];
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 1);
  lcd.print(strFrequency);
  lcd.display();
}
    
    // Shows the current power in percent (duty cycle)
void showPower() {
  char strPower[12];
  uint16_t currentPower = (uint16_t)(pwmPowerDuty * 100 / 255);
  sprintf(strPower, "%d%%  ", currentPower);
  lcd.setCursor(0, 1);
  lcd.print(strPower);
}
    
    // Shows the general current transmitter status
void showStatus() {
  char strFrequency[9];
  char str[20];
  lcd.clear();
  //checkQN8066();

  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 0);
  lcd.print(strFrequency);
  lcd.print("Mhz");
  lcd.setCursor(14, 0);
  lcd.print(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].desc);  // Mono Stereo
      
      // Audio Peak
  lcd.setCursor(0, 1);
  lcd.print("Pk:");
  lcd.print(tx.getAudioPeakValue());
  lcd.print("mV");
  tx.resetAudioPeak();
      
      // Power reference
  lcd.setCursor(13, 1);
  sprintf(str, "PA:%d%%", pwmPowerDuty * 100 / 255);
  lcd.print(str);
      
      // RDS
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1){
  sprintf(str, "PS:%s   PTY:%2d", tx.rdsGetPS(), tx.rdsGetPTY());
  lcd.setCursor(0, 2);
  lcd.print(str);
  lcd.setCursor(0, 3);
  sprintf(str, "RDS ERR: %d", tx.rdsGetError());
  lcd.print(str);
} else {     
  lcd.setCursor(0, 3);
  lcd.print("RDS:OFF");
}       
      
      // FSM code
  lcd.setCursor(14, 3);
  sprintf(str, "FSM:%d", tx.getFsmStateCode());
  lcd.print(str);


  // DEBUG - Monitoring time and I2C error 
  if ( (millis() - timeInTheAr) > 60000) { 
    countTime++;
    timeInTheAr = millis();
  }

  lcd.setCursor(10, 0);
  lcd.print(countI2CError);
  lcd.setCursor(8, 1);
  lcd.print(countTime);
  // END DEBUG
  
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
  return BT_MENU_PRESSED;
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
  while (key != BT_MENU_PRESSED) {
    if (key == -1) {
      if (txFrequency < 640)   // If less than 64 MHz
        txFrequency = 1080;
      else
        txFrequency -= STEP_FREQ;
    } else if (key == 1) {
      if (txFrequency > 1080)  // if more than 108 MHz
        txFrequency = 640;
      else
        txFrequency += STEP_FREQ;
    }
    switchTxFrequency(txFrequency);
    showFrequency();
    key = browseParameter();
  }
  menuLevel = 0;
}
      
      // Processes the change to a new power (PWM duty cycle)
void doPower() {
  showPower();
  int8_t key = browseParameter();
  while (key != BT_MENU_PRESSED) {
    if (key == -1) {
      if (pwmPowerDuty >= 25)
        pwmPowerDuty -= pwmDutyStep;
      else
        pwmPowerDuty = 0;
    } else if (key == 1) {
      if (pwmPowerDuty <= 225)
        pwmPowerDuty += pwmDutyStep;
      else
        pwmPowerDuty = 255;
    }
    pinMode(PWM_PA, OUTPUT); 
    analogWrite(PWM_PA, pwmPowerDuty);
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
  while (key != BT_MENU_PRESSED) {
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
      enablePWM(0);            // The PWM seems to interfere with the communication with the QN8066.
      lcd.setCursor(6, 1);
      lcd.print("<");      // it just indicates the edit mode
      doFrequency();
      enablePWM(pwmPowerDuty);          // Turn the PWM on again.
      break;
    case KEY_POWER:
      lcd.setCursor(6, 1);
      lcd.print("<");      // it just indicates the edit mode
      doPower();
      break;
    case KEY_MONO_STEREO:
      runAction([](uint8_t value) {
        tx.setTxMono(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_PRE_EMPHASIS:
      runAction([](uint8_t value) {
        tx.setPreEmphasis(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_RDS:
      runAction([](uint8_t value) {
        tx.rdsTxEnable(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      startRDS();
      break;
    case KEY_RDS_PTY:
      runAction([](uint8_t value) {
        tx.rdsSetPTY(value);
      },
                &keyValue[idxMenu], 1, 0, 11);
      break;
    case KEY_IMPEDANCE:
      runAction([](uint8_t value) {
        tx.setTxInputImpedance(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_SOFT_CLIP:
      runAction([](uint8_t value) {
        tx.setTxSoftClippingEnable(value);
      },
                &keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_SOFT_CLIP_THRESHOLD:
      runAction([](uint8_t value) {
        tx.setTxSoftClipThreshold(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_GAIN_PILOT:
      runAction([](uint8_t value) {
        tx.setTxPilotGain(value);
      },
                &keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_FREQ_DERIVATION:
      runAction([](uint8_t value) {
        tx.setTxFrequencyDerivation(value);
      },
                &keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_BUFFER_GAIN:
      runAction([](uint8_t value) {
        tx.setTxInputBufferGain(value);
      },
                &keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_RDS_FREQ_DEV:
      runAction([](uint8_t value) {
        tx.rdsSetFrequencyDerivation(value);
      },
                &keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_EXIT:
      return 0;
      break;
    default:
      break;
  }
  saveAllTransmitterInformation();  // Saves the current modified data to the EEPROM
  return 1;
}


void sendRDS() {
  // PS refreshing control
  if ((millis() - rdsTimePS) > RDS_PS_REFRESH_TIME) {
    if (idxRdsPS > lastRdsPS) idxRdsPS = 0;
    delay(100);
    tx.rdsSendPS(rdsPSmsg[idxRdsPS]);
    idxRdsPS++;
    rdsTimePS = millis();
  }

  // RT refreshing control
  if ((millis() - rdsTimeRT) > RDS_RT_REFRESH_TIME) {
    if (idxRdsRT > lastRdsRT) idxRdsRT = 0;
    delay(100);
    tx.rdsSendRT(rdsRTmsg[idxRdsRT]);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    idxRdsRT++;
    rdsTimeRT = millis();
  }

  // Date Time Service refreshing control
  if ((millis() - rdsDateTime) > RDS_DT_REFRESH_TIME) {
    delay(100);
    // To use the function (service) below, you will need to add an integrated clock to your 
    // system that provides the date and time to the system. The following example presents 
    // only a fixed date and time and is intended solely to illustrate the use of the function.
    tx.rdsSendDateTime(2024, 8, 30, 13, 01, 0);  // Sends Year = 2024; month = 8; day = 29; At 12:45 (local time)    
    rdsDateTime = millis();
  }
}


int8_t checkEncoder() {

  int8_t action;

  if ( digitalRead(BT_MENU) == LOW ) { 
     action =  BT_MENU_PRESSED;
     delay(250);                   // Try to avoid double click or debounce 
  }
  else if ( encoderCount == 1) 
    action =  ENCODER_RIGHT;
  else if ( encoderCount == -1) 
    action =  ENCODER_LEFT;
  else
    action = ENCODER_NO_ACTION;  

  encoderCount = 0;

  return action;
}

void loop() {
  int8_t key;
  if (menuLevel == 0) {
    showStatus();
    while ((key = checkEncoder()) == ENCODER_NO_ACTION) {

      if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1  && !stopRDSforWhile )   {
        sendRDS();
      }

      if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1 && stopRDSforWhile && (millis() - stopRDSTime) > STOP_RDS_TIME ) { 
        stopRDSforWhile = false; 
        tx.rdsTxEnable(true);
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
    if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) tx.rdsTxEnable(false);
   if ( key == BT_MENU_PRESSED  ) {  // Menu Pressed
      menuLevel = 1;
    } 
  } else if (menuLevel == 1) {
    showMenu(menuIdx);
    key = browseParameter();
    while (key != BT_MENU_PRESSED) {
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
