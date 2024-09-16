/*
 
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino 
  This version does not use RDS functions
  This sketch uses an Arduino or similar with LCD16X02.
  This sketch wotks with ATMEGA328 and LGT8F328 based boards. So, you can use it on Arduino Nano, Pro Mini, Uno and Nano Clone.



  The LGT8F328P is a microcontroller that is designed to be compatible with the popular ATmega328P, 
  which is widely used in Arduino boards like the Arduino Uno. The LGT8F328P is manufactured by LogicGreen 
  and is often considered a more cost-effective or feature-rich alternative to the ATmega328P.

  Use LTG8FX manager board to compile this sketch. 

  ATTENTION: TO RESET the EEPROM: Turn your receiver on with the MENU push button pressed.

  Read more at https://pu2clr.github.io/QN8066/

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


  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Aug 2024.
*/

#include <QN8066.h>
#include <EEPROM.h>
#include "Rotary.h"
#include <LiquidCrystal.h>

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
#define PUSH_MIN_DELAY 200
#define STATUS_REFRESH_TIME 5000

volatile int encoderCount = 0;

int8_t lcdPage = 0;
long showStatusTime = millis();
uint8_t menuLevel = 0;
const uint8_t app_id = 80;  // Useful to check the EEPROM content before processing useful data
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


TableValue tabMonoStereo[] = {
  { 0, "Stereo" },  // 0
  { 1, "Mono" }   // 1 - See QN8066 data sheet
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
  KEY_IMPEDANCE,            // 5
  KEY_SOFT_CLIP_ENABLE,     // 6
  KEY_SOFT_CLIP_THRESHOLD,  // 7
  KEY_GAIN_PILOT,           // 8
  KEY_FREQ_DERIVATION,      // 9
  KEY_BUFFER_GAIN,          // 10
  KEY_MAIN_SCREEN           // 11
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
  { 2, tabImpedance },             // KEY_IMPEDANCE
  { 0, tabTxSoftClipEnable },      // KEY_SOFT_CLIP_ENABLE
  { 1, tabTxSoftClipThreshold },   // KEY_SOFT_CLIP_THRESHOLD
  { 2, tabGainTxPilot },           // KEY_GAIN_PILOT
  { 2, tabTxFrequencyDeviation },  // KEY_FREQ_DERIVATION
  { 1, tabTxBufferGain },          // KEY_BUFFER_GAIN
  { 0, NULL }                      // KEY_MAIN_SCREEN
};

uint16_t txFrequency = 1069;  // Default frequency is 106.9 MHz

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
  tx.setTxMono(keyValue[KEY_MONO_STEREO].value[keyValue[KEY_MONO_STEREO].key].idx);
  tx.setTxInputBufferGain(keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].idx);
  tx.setTxFrequencyDerivation(keyValue[KEY_FREQ_DERIVATION].value[keyValue[KEY_FREQ_DERIVATION].key].idx);
  tx.setPAC(keyValue[KEY_PAC].value[keyValue[KEY_PAC].key].idx);

  showStatus(lcdPage);

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
  EEPROM.update(eeprom_address + 7, keyValue[KEY_IMPEDANCE].key);
  EEPROM.update(eeprom_address + 8, keyValue[KEY_SOFT_CLIP_ENABLE].key);
  EEPROM.update(eeprom_address + 9, keyValue[KEY_SOFT_CLIP_THRESHOLD].key);
  EEPROM.update(eeprom_address + 10, keyValue[KEY_GAIN_PILOT].key);
  EEPROM.update(eeprom_address + 11, keyValue[KEY_FREQ_DERIVATION].key);
  EEPROM.update(eeprom_address + 12, keyValue[KEY_BUFFER_GAIN].key);
  EEPROM.update(eeprom_address + 15, keyValue[KEY_PAC].key);
}
// Read the previous transmitter setup
void readAllTransmitterInformation() {
  txFrequency = EEPROM.read(eeprom_address + 1) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 2);
  pwmPowerDuty = EEPROM.read(eeprom_address + 3);
  keyValue[KEY_MONO_STEREO].key = EEPROM.read(eeprom_address + 4);
  keyValue[KEY_PRE_EMPHASIS].key = EEPROM.read(eeprom_address + 5);
  keyValue[KEY_IMPEDANCE].key = EEPROM.read(eeprom_address + 7);
  keyValue[KEY_SOFT_CLIP_ENABLE].key = EEPROM.read(eeprom_address + 8);
  keyValue[KEY_SOFT_CLIP_THRESHOLD].key = EEPROM.read(eeprom_address + 9);
  keyValue[KEY_GAIN_PILOT].key = EEPROM.read(eeprom_address + 10);
  keyValue[KEY_FREQ_DERIVATION].key = EEPROM.read(eeprom_address + 11);
  keyValue[KEY_BUFFER_GAIN].key = EEPROM.read(eeprom_address + 12);
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
  } else {
    sprintf(str, "BG:%s", keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].desc);
    lcd.setCursor(0, 0);
    lcd.print(str);
    lcd.setCursor(0, 1);
    sprintf(str, "PIL.:%s", keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].desc);
    lcd.print(str);
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
    case KEY_MAIN_SCREEN:
      return 0;
      break;
    default:
      break;
  }
  saveAllTransmitterInformation();  // Saves the current modified data to the EEPROM
  return 1;
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
      // Refresh Status
      if ((millis() - showStatusTime) > STATUS_REFRESH_TIME) {
        showStatus(lcdPage);
        showStatusTime = millis();
      }
    }

    if (key == ENCODER_LEFT) {  // Down Pressed
      lcdPage--;
      if (lcdPage < 0) lcdPage = 2;
      showStatus(lcdPage);
    } else if (key == ENCODER_RIGHT) {  // Up Pressed
      lcdPage++;
      if (lcdPage > 2) lcdPage = 0;
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

  delay(5);
}
