/*
 
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD16X02.

  The LGT8F328P is a microcontroller that is designed to be compatible with the popular ATmega328P, 
  which is widely used in Arduino boards like the Arduino Uno. The LGT8F328P is manufactured by LogicGreen 
  and is often considered a more cost-effective or feature-rich alternative to the ATmega328P.

  Use LTG8FX manager board to compile this sketch. 

  This sketch uses the DS1302 RTC (Real Time Clock) to provide RDS Date and Time service.

  ATTENTION: TO RESET the EEPROM: Turn your receiver on with the MENU push button pressed.

  Read more at https://pu2clr.github.io/QN8066/
  Check the RDS functions documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html

  Wire up on Arduino Nano or LGT8F328

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
  | ------------------------- | ------------------------- | ------------- |
  |    DS1302 RTC             | RST (RESET)               |    A1/D15     |
  |                           | DATA                      |    A2/D16     | 
  |                           | CLOCK                     |    A3/D17     |    
  | --------------------------| ------------------------- | --------------|
  | QN8066                    |                           |               |
  | DIY KIT 5˜7W              | ------------------------- | --------------|
  |                           | SDA                       |     A4        |
  |                           | SCLK                      |     A5        |
  |                           | PWM                       |     D9        |
  | --------------------------| --------------------------| --------------|
  | Button                    |                           |               |
  |                           | Menu                      |     D8        |
  | Encoder                   |                           |               |
  |                           | Left                      |     D2        |
  |                           | Right                     |     D3        |
  | --------------------------| --------------------------|---------------|

  IMPORTANT: 

  Usage and Operation Instructions:

  1) This program uses the internal ATmega328 EEPROM to store the transmitter configuration parameters. 
     If necessary, press the MENU button when turning on the system to reset the EEPROM and 
     return the transmitter to the default settings.
  2) To access the MENU, quickly press the knob or rotary shaft of the encoder down. 
  3) Select the menu item you wish to adjust by turning the encoder clockwise or counterclockwise. 
  4) Once you've chosen the item you want to change, press the encoder again. 
  5) Turn the encoder clockwise or counterclockwise to select the new parameters. 
  6) Finally, press the encoder one more time to confirm and apply the parameter change.

  It is important to highlight that if RDS is activated/enabled, it will be temporarily disabled while
  the user interacts with the MENU and parameter adjustments. Once the adjustments are complete, the 
  RDS will resume functioning after a few seconds. This occurs to improve the system's responsiveness, 
  as RDS processing requires synchronization and a processing load that can, to some extent, impact the
  user interface.

  If you have RDS enabled, it's possible that the first movement of the encoder or button may not respond
  immediately. As soon as the system responds for the first time, RDS is temporarily disabled, and the 
  system’s response becomes more fluid.


  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Aug 2024.
*/

#include <QN8066.h>
#include <EEPROM.h>
#include "Rotary.h"
#include <LiquidCrystal.h>
#include <Ds1302.h>   // Real Time Clock based on DS1302

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

// RTC DS1302 Control PINS
#define CLK_PIN     14      // Using the D17/A3 for clock
#define DATA_PIN    15      // Using the D16/A2 for data
#define RESET_PIN   16      // Using the D15/A1 for reset the DS1302 device




#define ENCODER_NO_ACTION 0
#define ENCODER_LEFT -1  
#define ENCODER_RIGHT 1
#define BT_MENU_PRESSED 2  

// Enconder PINs
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

#define STEP_FREQ 1
#define PUSH_MIN_DELAY 200

#define STATUS_REFRESH_TIME 5000
#define STOP_RDS_TIME 10000


volatile int encoderCount = 0;

bool stopRDSforWhile = false;
long stopRDSTime = millis();

int8_t lcdPage = 0;
long showStatusTime = millis();

uint8_t menuLevel = 0;

const uint8_t app_id = 86;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;

// The PWM duty can be set from 25 to 255 where 255 is the max power (7W) .
// So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPowerDuty = 0;  // Initial power/duty.
uint8_t pwmDutyStep = 10;   // PWM Duty increment and decrement step

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
  { 0, "10K" },  // 0
  { 1, "20K" },  // 1
  { 2, "40K" },  // 2
  { 3, "80K" }   // 3
};

TableValue tabGainTxPilot[] = {
  { 7,  "7%" },   // 0
  { 8,  "8%" },   // 1
  { 9,  "9%" },   // 2
  { 10, "10%" }  // 3
};

TableValue tabTxSoftClipEnable[] = {
  { 0, "Disable" },  // 0
  { 1, "Enable" }   // 1
};

TableValue tabTxSoftClipThreshold[] = {
  { 0, "3dB" },    // 0
  { 1, "4.5dB" },  // 1
  { 2, "6dB" },    // 2
  { 3, "9dB" }     // 3
};

TableValue tabTxFrequencyDeviation[] = {
  { 60,  "41,5kHz" },   // 0
  { 87,  "60,0kHz" },   // 1
  { 108, "74,5kHz" },  // 2
  { 120, "92,8kHz" },  // 3
  { 140, "96,6kHz" },  // 4
  { 160, "110,4kHz" }   // 5
};

TableValue tabTxBufferGain[] = {
  { 0, " 3dB" },   // 0
  { 1, " 6dB" },   // 1
  { 2, " 9dB" },   // 2
  { 3, "12dB" },  // 3
  { 4, "15dB" },  // 4
  { 5, "18dB" }   // 5
};

TableValue tabPreEmphasis[] = {
  { 0, "50 us" },  // 0
  { 1, "75 us" }   // 1
};

TableValue tabRDS[] = {
  { 0, "Disable" },  // 0
  { 1, "Enable" }   // 1
};

TableValue tabRdsPty[] {     // See https://en.wikipedia.org/wiki/Radio_Data_System if you want to change this table 
  { 0, "No program"},
  { 1, "News"},
  { 3, "Inf./Sports"},
  { 4, "Sport/Talk"},
  { 5, "Education/Rock"},
  { 7, "Cult./Adult hits"},
  { 8, "Science/S.rock"},
  {10, "Pop Mus/Country" },
  {16, "Weather/Rhythm"},
  {20, "Religion" },
  {29, "Doc./Weather"},
  {30, "Alarm/Emerg."}
};

TableValue tabMonoStereo[] = {
  { 0, "Stereo" },  // 0
  { 1, "Mono" }   // 1 - See QN8066 data sheet
};


TableValue txRdsFreqDev[] = {
  {  6, "2,1kHz"},  // 0
  {  8, "3,0kHz"},  // 1
  { 13, "4,5kHz" },  // 2
  { 26, "9,1kHz" },  // 3
  { 39, "13,6kHz" },  // 4
  { 52, "18,2kHz" }   // 5
};


TableValue txPac[] = {
  { 24, " 92 dBu"}, // 0
  { 30, " 97 dBu"}, // 1
  { 40, "106 dBu"}, // 2
  { 50, "115 dBu"}, // 3
  { 55, "120 dBu"}  // 4
};




// Menu Itens
const char *menu[] = { "Frequency",
                       "Power",
                       "PAC Power Target",
                       "Stereo/Mono",
                       "Pre-emphasis",
                       "RDS",
                       "RDS PTY",
                       "RDS Freq. Dev.",                       
                       "Impedance",
                       "Sft Clip. Enable",
                       "Sft Clip. Thres.",
                       "Gain Pilot",
                       "Freq. Deriv.",
                       "Buffer gain",
                       "Main Screen" };
int8_t menuIdx = 0;
const int lastMenu = (sizeof(menu) / sizeof(menu[0])) - 1;  // Laste menu item position

// Define the enum with the corresponding Menu Itens QN8066 register values
enum MenuKeys {
  KEY_FREQUENCY,            // 0
  KEY_POWER,                // 1
  KEY_PAC,                  // 2
  KEY_MONO_STEREO,          // 3
  KEY_PRE_EMPHASIS,         // 4
  KEY_RDS,                  // 5
  KEY_RDS_PTY,              // 5
  KEY_RDS_FREQ_DEV,         // 7  
  KEY_IMPEDANCE,            // 8
  KEY_SOFT_CLIP_ENABLE,     // 9
  KEY_SOFT_CLIP_THRESHOLD,  // 10
  KEY_GAIN_PILOT,           // 11
  KEY_FREQ_DERIVATION,      // 12
  KEY_BUFFER_GAIN,          // 13
  KEY_MAIN_SCREEN           // 14
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
  { 4, txPac},                     // KEY_PAC
  { 0, tabMonoStereo },            // KEY_MONO_STEREO
  { 1, tabPreEmphasis },           // KEY_PRE_EMPHASIS
  { 0, tabRDS },                   // KEY_RDS
  { 6, tabRdsPty},                 // KEY_RDS_PTY 
  { 2, txRdsFreqDev },             // KEY_RDS_FREQ_DEV  
  { 2, tabImpedance },             // KEY_IMPEDANCE
  { 0, tabTxSoftClipEnable },      // KEY_SOFT_CLIP_ENABLE
  { 1, tabTxSoftClipThreshold },   // KEY_SOFT_CLIP_THRESHOLD
  { 2, tabGainTxPilot },           // KEY_GAIN_PILOT
  { 2, tabTxFrequencyDeviation },  // KEY_FREQ_DERIVATION
  { 1, tabTxBufferGain },          // KEY_BUFFER_GAIN
  { 0, NULL }                      // KEY_MAIN_SCREEN
};

uint16_t txFrequency = 1069;  // Default frequency is 106.9 MHz
// Station Name (PS) messages
char *rdsPSmsg[] = { (char *)"PU2CLR \r",
                     (char *)"QN8066 \r",
                     (char *)"ARDUINO\r",
                     (char *)"LIBRARY\r",
                     (char *)"FM TX  \r" };

// Radio Text (RT) messages
// Apparently, some receivers seem to work better when the special character 0xD (\r) 
// is added at the end of the string (message). Therefore, although it is technically 
// possible to work with messages up to 64 characters long, it is recommended to keep 
// the message length within 32 characters by default, reserving the 32nd character for
// the '\r' as shown below.
char *rdsRTmsg[] = { (char *)"PU2CLR QN8066 ARDUINO LIBRARY  \r",
                     (char *)"FM TRANSMITTER WITH RDS SERVICE\r",
                     (char *)"github.com/pu2clr/QN8066       \r",
                     (char *)"FM TX for Enthusiasts          \r",
                     (char *)"QN8066 HOMEBREW FM TRANSMITTER \r" };

const uint8_t lastRdsPS = (sizeof(rdsPSmsg) / sizeof(rdsPSmsg[0])) - 1;
const uint8_t lastRdsRT = (sizeof(rdsRTmsg) / sizeof(rdsRTmsg[0])) - 1;

uint8_t idxRdsPS = 0;
uint8_t idxRdsRT = 0;

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 19000
#define RDS_DT_REFRESH_TIME 60000 // Date and Time Service

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

// *** DEBUG
long  timeInTheAr = millis();
long  countI2CError = 0;  
long  countTime = 0;
// ****

// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

Ds1302 rtc(RESET_PIN, CLK_PIN, DATA_PIN);
Ds1302::DateTime dt;


void setup() {

  rtc.init(); //Inicializa o rtc

  // To set the RTC, uncomment this block of lines, compile and upload the sketch to the Arduino. 
  // Once the clock is set, comment out the lines again, compile and upload the sketch.  

  /*
  dt.year = 24;
  dt.month = 9;
  dt.day = 14;
  dt.dow = 1;
  dt.hour = 12;
  dt.minute = 4;
  dt.second = 10;
  rtc.setDateTime(&dt);
  */
  
  
  
  pinMode(PWM_PA, OUTPUT);  // Sets the Arduino PIN to operate with with PWM
  pinMode(BT_MENU, INPUT_PULLUP);

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);  


  // controlling encoder via interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);



  tx.setI2CFastMode();
  lcd.begin(16, 2);

  // If you want to reset the eeprom, keep the BT_MENU button pressed during statup
  if (digitalRead(BT_MENU) == LOW) {
    lcd.clear();
    EEPROM.write(eeprom_address, 0);
    lcd.setCursor(0, 0);
    lcd.print("RESET TO DEFAULT");
    delay(3000);
    lcd.clear();
  }

  showSplash();
  delay(1000);

  lcd.clear();

  checkQN8066();

  // Check the EEPROM content. If it contains valid data, read it (previous setup).
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllTransmitterInformation();
  } else {
    // Defult values
    txFrequency = 1069;
    pwmPowerDuty = 0;
    saveAllTransmitterInformation();
  }

  enablePWM(0);  // PWM disable

  tx.setup();
  tx.setTX(txFrequency);
  delay(300);

  tx.resetAudioPeak();

  // Due to the architecture of the KIT, the PWM interferes with I2C communication.
  // Therefore, before changing the transmitter's configuration parameters, it must be disabled (Duty 0).

    // Sets the transmitter with the previous setup parameters
  tx.setTxInputImpedance(keyValue[KEY_IMPEDANCE].value[keyValue[KEY_IMPEDANCE].key].idx);  // 40Kohm
  tx.setTxPilotGain(keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].idx);
  tx.setTxSoftClippingEnable(keyValue[KEY_SOFT_CLIP_ENABLE].value[keyValue[KEY_SOFT_CLIP_ENABLE].key].idx);
  tx.setTxSoftClipThreshold(keyValue[KEY_SOFT_CLIP_THRESHOLD].value[keyValue[KEY_SOFT_CLIP_THRESHOLD].key].idx);
  tx.setPreEmphasis(keyValue[KEY_PRE_EMPHASIS].value[keyValue[KEY_PRE_EMPHASIS].key].idx);
  tx.rdsTxEnable(keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx);
  tx.setTxMono(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].idx);
  tx.setTxInputBufferGain(keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].idx);
  tx.rdsSetFrequencyDerivation(keyValue[KEY_RDS_FREQ_DEV].value[keyValue[KEY_RDS_FREQ_DEV].key].idx);
  tx.setTxFrequencyDerivation(keyValue[KEY_FREQ_DERIVATION].value[keyValue[KEY_FREQ_DERIVATION].key].idx);
  tx.setPAC(keyValue[KEY_PAC].value[keyValue[KEY_PAC].key].idx);

  showStatus(lcdPage);

  // Checking RDS setup
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
    startRDS();
    sendRDS();                             // Control the RDS PS and RT messages with this function
  }


  // Adjust clock divider (Timer0) 
  // TCCR0B = (TCCR0B & 0b11111000) | 0x02; // Increases the clock of PWM to 62.5 kHz.

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


void startRDS() {
    uint8_t ptyIdx = keyValue[KEY_RDS_PTY].value[keyValue[KEY_RDS_PTY].key].idx;
    tx.rdsInitTx(0x8,0x1,0x9B, ptyIdx, 50, 4);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
}

void checkQN8066() {
  if (!tx.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1)
      ;
  }
}

// Saves current transmitter setup
void saveAllTransmitterInformation() {
  // The update function/method writes data only if the current data is not equal to the stored data.
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, txFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 2, txFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 3, pwmPowerDuty);
  EEPROM.update(eeprom_address + 4, keyValue[KEY_MONO_STEREO].key);
  EEPROM.update(eeprom_address + 5, keyValue[KEY_PRE_EMPHASIS].key);
  EEPROM.update(eeprom_address + 6, keyValue[KEY_RDS].key);
  EEPROM.update(eeprom_address + 7, keyValue[KEY_IMPEDANCE].key);
  EEPROM.update(eeprom_address + 8, keyValue[KEY_SOFT_CLIP_ENABLE].key);
  EEPROM.update(eeprom_address + 9, keyValue[KEY_SOFT_CLIP_THRESHOLD].key);
  EEPROM.update(eeprom_address + 10, keyValue[KEY_GAIN_PILOT].key);
  EEPROM.update(eeprom_address + 11, keyValue[KEY_FREQ_DERIVATION].key);
  EEPROM.update(eeprom_address + 12, keyValue[KEY_BUFFER_GAIN].key);
  EEPROM.update(eeprom_address + 13, keyValue[KEY_RDS_FREQ_DEV].key);
  EEPROM.update(eeprom_address + 14, keyValue[KEY_RDS_PTY].key);
  EEPROM.update(eeprom_address + 15, keyValue[KEY_PAC].key);
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
  keyValue[KEY_SOFT_CLIP_ENABLE].key = EEPROM.read(eeprom_address + 8);
  keyValue[KEY_SOFT_CLIP_THRESHOLD].key = EEPROM.read(eeprom_address + 9);
  keyValue[KEY_GAIN_PILOT].key = EEPROM.read(eeprom_address + 10);
  keyValue[KEY_FREQ_DERIVATION].key = EEPROM.read(eeprom_address + 11);
  keyValue[KEY_BUFFER_GAIN].key = EEPROM.read(eeprom_address + 12);
  keyValue[KEY_RDS_FREQ_DEV].key = EEPROM.read(eeprom_address + 13);
  keyValue[KEY_RDS_PTY].key = EEPROM.read(eeprom_address + 14);
  keyValue[KEY_PAC].key = EEPROM.read(eeprom_address + 15);
}

// Enable or disable PWM duty cycle
void enablePWM(uint8_t value) {
  pinMode(PWM_PA, OUTPUT);
  delay(100);
  analogWrite(PWM_PA, value);  // Turn PA off
  delay(100);
}
// Switches the the current frequency to a new frequency
void switchTxFrequency(uint16_t freq) {
  tx.setTX(txFrequency = freq);
  showFrequency();
}
// Shows the first message after turn the transmitter on
void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
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
  char strPower[7];
  // uint16_t currentPower = (uint16_t)(pwmPowerDuty * 7 / 255);
  uint16_t currentPower = (uint16_t)(pwmPowerDuty * 100 / 255);
  sprintf(strPower, "%d%%  ", currentPower);
  lcd.setCursor(0, 1);
  lcd.print(strPower);
}
// Shows the general current transmitter status
void showStatus(uint8_t page) {
  char strFrequency[7];
  char str[20];

  lcd.clear();
  checkQN8066();
  if (page == 0) {
    tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
    lcd.setCursor(0, 0);
    lcd.print(strFrequency);
    lcd.print("MHz");
    lcd.setCursor(10, 0);
    // lcd.print(  tabMonoStereo[idxStereoMono].desc );
    lcd.print(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].desc);  // Mono Stereo
    lcd.setCursor(0, 1);
    lcd.print(tx.getAudioPeakValue());
    lcd.print("mV");
    lcd.setCursor(10, 1);
    sprintf(str, "PA:%d%%", pwmPowerDuty * 100 / 255);
    lcd.print(str);
    tx.resetAudioPeak();
  } else if (page == 1) {
    lcd.setCursor(0, 0);
    sprintf(str, "FSM: %d", tx.getFsmStateCode());
    lcd.print(str);
    sprintf(str, "RIN:%s", keyValue[KEY_IMPEDANCE].value[keyValue[KEY_IMPEDANCE].key].desc);
    lcd.setCursor(9, 0);
    lcd.print(str);
    lcd.setCursor(0, 1);
    sprintf(str, "DEV.: %s", keyValue[KEY_FREQ_DERIVATION].value[keyValue[KEY_FREQ_DERIVATION].key].desc);
    lcd.print(str);
  } else if (page == 2) {
    sprintf(str, "BG:%s", keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].desc);
    lcd.setCursor(0, 0);
    lcd.print(str);
    lcd.setCursor(0, 1);
    sprintf(str, "PIL.:%s", keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].desc);
    lcd.print(str);
  } else if (page == 3) {
    sprintf(str, "PS:%.6s PTY:%2d", tx.rdsGetPS() , tx.rdsGetPTY());
    lcd.setCursor(0, 0);
    lcd.print(str);
    lcd.setCursor(0, 1);
    sprintf(str, "RT:%.14s", rdsRTmsg[idxRdsRT] );
    lcd.print(str);
  }
  else if ( page == 4) {
    lcd.setCursor(0, 0);      
    lcd.print("RDS DT Service");
    if (!dt.dow) rtc.getDateTime(&dt);
    sprintf(str,"%2.2d/%2.2d/%2.2d %2.2d:%2.2d",dt.year,dt.month,dt.day,dt.hour,dt.minute);
    lcd.setCursor(0, 1);
    lcd.print(str);
  } else {
    lcd.setCursor(0, 0);      
    lcd.print("Time on the Air");
    sprintf(str, "%ld min.", countTime);
    lcd.setCursor(0, 1);
    lcd.print(str);
  } 

  // DEBUG - Monitoring time and I2C error 
  if ( (millis() - timeInTheAr) > 60000) { 
    countTime++;
    timeInTheAr = millis();
  }

  lcd.display();
}
// Shows the given parameter to be updated
void showParameter(char *desc) {
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(desc);
  lcd.print("<");
  lcd.display();
}
// Browse the parameters by polling the navigator buttons returns -1 (left/down), 0 (if Menu pressed), 1 (right/up).
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
      if (txFrequency < 640)  // If less than 64 MHz
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
  // menuLevel = 0;
}
// // Processes the change to a new power (PWM duty cycle)
void doPower() {
  showPower();
  int8_t key = browseParameter();
  while (key != BT_MENU_PRESSED) {
    if (key == ENCODER_LEFT) {
      if (pwmPowerDuty >= 25)
        pwmPowerDuty -= pwmDutyStep;
      else
        pwmPowerDuty = 0;
    } else if (key == ENCODER_RIGHT) {
      if (pwmPowerDuty <= 225)
        pwmPowerDuty += pwmDutyStep;
      else
        pwmPowerDuty = 255;
    }
    enablePWM(pwmPowerDuty);
    showPower();
    key = browseParameter();
  }
  // menuLevel = 0;
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
      enablePWM(0);  // The PWM seems to interfere with the communication with the QN8066.
      lcd.setCursor(9, 1);
      lcd.print("<<");  // it just indicates the edit mode
      doFrequency();
      enablePWM(pwmPowerDuty);          // Turn the PWM on again.
      break;
    case KEY_POWER:
      lcd.setCursor(9, 1);
      lcd.print("<<");  // it just indicates the edit mode
      doPower();
      break;
    case KEY_PAC:
      runAction([](uint8_t value) {tx.setPAC(value);}, &keyValue[idxMenu], 1, 0, 4);
      break;      
    case KEY_MONO_STEREO:
      runAction([](uint8_t value) {tx.setTxMono(value);}, &keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_PRE_EMPHASIS:
      runAction([](uint8_t value) {tx.setPreEmphasis(value);},&keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_RDS:
      runAction([](uint8_t value) {tx.rdsTxEnable(value);}, &keyValue[idxMenu], 1, 0, 1);
      startRDS();
      break;
    case KEY_RDS_PTY: 
      runAction([](uint8_t value) {tx.rdsSetPTY(value);}, &keyValue[idxMenu], 1, 0, 11);
      break;
    case KEY_IMPEDANCE:
      runAction([](uint8_t value) {tx.setTxInputImpedance(value);}, &keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_SOFT_CLIP_ENABLE:
      runAction([](uint8_t value) {tx.setTxSoftClippingEnable(value);}, &keyValue[idxMenu], 1, 0, 1);
      break;
    case KEY_SOFT_CLIP_THRESHOLD:
      runAction([](uint8_t value) {tx.setTxSoftClipThreshold(value);},&keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_GAIN_PILOT:
      runAction([](uint8_t value) {tx.setTxPilotGain(value);}, &keyValue[idxMenu], 1, 0, 3);
      break;
    case KEY_FREQ_DERIVATION:
      runAction([](uint8_t value) {tx.setTxFrequencyDerivation(value);},&keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_BUFFER_GAIN:
      runAction([](uint8_t value) {tx.setTxInputBufferGain(value);}, &keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_RDS_FREQ_DEV:
      runAction([](uint8_t value) {tx.rdsSetFrequencyDerivation(value);}, &keyValue[idxMenu], 1, 0, 5);
      break;
    case KEY_MAIN_SCREEN:
      return 0;
      break;
    default:
      break;
  }
  saveAllTransmitterInformation();  // Saves the current modified data to the EEPROM
  return 1;
}

/**
   Here are some recommended configurations for testing the RDS features
   1: Stereo = On
   2: Pre-emphasis = 75 us
   3: RDS = Enable
   4: Impedance = 20K
   5: Sft Clip.. = Disable
   6: Thres = 3dB
   7: Gain Pilot = 10%
   8: Dreq. Deriv. 74.52
   9: Buffer gain = 3dB
  10: RDS Freq. Dev. 4.55kHz
*/

void sendRDS() {

  // PS refreshing control
  if ((millis() - rdsTimePS) > RDS_PS_REFRESH_TIME) {
    tx.rdsSendPS(rdsPSmsg[idxRdsPS]);
    idxRdsPS++;
    if (idxRdsPS > lastRdsPS) idxRdsPS = 0;
    rdsTimePS = millis();
  }

  // RT refreshing control
  if ((millis() - rdsTimeRT) > RDS_RT_REFRESH_TIME) {
    tx.rdsSendRT(rdsRTmsg[idxRdsRT]);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    idxRdsRT++;
    if (idxRdsRT > lastRdsRT) idxRdsRT = 0;
    rdsTimeRT = millis();
  }

  // Date Time Service refreshing control
  if ((millis() - rdsDateTime) > RDS_DT_REFRESH_TIME) {
    rtc.getDateTime(&dt);
    if (!dt.dow) rtc.getDateTime(&dt);
    tx.rdsSendDateTime(dt.year + 2000, dt.month, dt.day, dt.hour, dt.minute, 0);
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

// Main loop
void loop() {
  int8_t key;
  if (menuLevel == 0) {
    showStatus(lcdPage);
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
        showStatus(lcdPage);
        showStatusTime = millis();
      }
    }
    // If you RDS is configured, stop for a while the RDS processing to improve the encoder and buttons response. 
    stopRDSforWhile = true;
    if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) tx.rdsTxEnable(false);
    if (key == ENCODER_LEFT) {  // Down Pressed
      lcdPage--;
      if (lcdPage < 0) lcdPage = 5;
      showStatus(lcdPage);
    } else if (key == ENCODER_RIGHT) {  // Up Pressed
      lcdPage++;
      if (lcdPage > 5) lcdPage = 0;
      showStatus(lcdPage);
    } else if ( key == BT_MENU_PRESSED  ) {  // Menu Pressed
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
