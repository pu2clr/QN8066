/*
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD16X02.

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
  2. On the main screen, click the UP or DOWN button to display information pages about the 
     current state of the transmitter.
  3. Press the MENU button to enter the parameter navigation mode.
  3.1. At this level, press UP or DOWN to select the option or parameter you want to change.
  3.2. Press the MENU button again to enter parameter editing mode.
  3.3. Press the UP and DOWN buttons to change the parameters.
  3.4. Press the MENU button to confirm and save the selected parameter. At this point, the
       menu returns to the previous level, allowing you to select another parameter to change.
  4. The "Main Screen" option returns the system to the initial screen. Remember that at this 
    point, you can change pages by pressing UP or DOWN. This way, you can check the changes 
    made.     

  Check the RDS functions documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html

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

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

// Enconder PINs
#define BT_MENU 8
#define BT_UP 10
#define BT_DOWN 11
#define PWM_PA 9


#define BT_NO_PRESSED 7    // 111
#define BT_DOWN_PRESSED 6  // 110
#define BT_UP_PRESSED 5    // 101
#define BT_MENU_PRESSED 3  // 011

#define STEP_FREQ 1
#define PUSH_MIN_DELAY 200

#define STATUS_REFRESH_TIME 4000

int8_t lcdPage = 0;
long showStatusTime = millis();

long i2c_monitor_time = millis();

uint8_t menuLevel = 0;

const uint8_t app_id = 86;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;

// The PWM duty can be set from 25 to 255 where 255 is the max power (7W) .
// So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPowerDuty = 50;  // Initial power/duty.
uint8_t pwmDutyStep = 25;   // PWM Duty increment and decrement step

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
  { 7, "7%" },   // 0
  { 8, "8%" },   // 1
  { 9, "9%" },   // 2
  { 10, "10%" }  // 3
};

TableValue tabTxSoftClipEnable[] = {
  { 0, "Disable " },  // 0
  { 1, "Enable  " }   // 1
};

TableValue tabTxSoftClipThreshold[] = {
  { 0, "3dB" },    // 0
  { 1, "4.5dB" },  // 1
  { 2, "6dB" },    // 2
  { 3, "9dB" }     // 3
};

TableValue tabTxFrequencyDeviation[] = {
  { 60, " 41,5kHz" },   // 0
  { 87, " 60,0kHz" },   // 1
  { 108, " 74,5kHz" },  // 2
  { 120, " 92,8kHz" },  // 3
  { 140, " 96,6kHz" },  // 4
  { 160, "110,4kHz" }   // 5
};

TableValue tabTxBufferGain[] = {
  { 0, "3dB" },   // 0
  { 1, "6dB" },   // 1
  { 2, "9dB" },   // 2
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
  { 1, "Enable " }   // 1
};

TableValue tabMonoStereo[] = {
  { 0, "Stereo" },  // 0
  { 1, "Mono  " }   // 1 - See QN8066 data sheet
};


TableValue txRdsFreqDev[] = {
  {  6, " 2,1kHz "},  // 0
  {  8, " 3,0kHz "},  // 1
  { 13, " 4,5kHz" },  // 2
  { 26, " 9,1kHz" },  // 3
  { 39, "13,6kHz" },  // 4
  { 52, "18,2kHz" }   // 5
};


// Menu Itens
const char *menu[] = { "Frequency",
                       "Power",
                       "Stereo/Mono",
                       "Pre-emphasis",
                       "RDS",
                       "Inpedance",
                       "Sft Clip. Enable",
                       "Sft Clip. Thres.",
                       "Gain Pilot",
                       "Freq. Deriv.",
                       "Buffer gain",
                       "RDS Freq. Dev.",
                       "Main Screen" };
int8_t menuIdx = 0;
const int lastMenu = (sizeof(menu) / sizeof(menu[0])) - 1;  // Laste menu item position

// Define the enum with the corresponding Menu Itens QN8066 register values
enum MenuKeys {
  KEY_FREQUENCIA,           // 0
  KEY_POWER,                // 1
  KEY_MONO_ESTEREO,         // 2
  KEY_PRE_EMPHASIS,         // 3
  KEY_RDS,                  // 4
  KEY_INPEDANCE,            // 5
  KEY_SOFT_CLIP_ENABLE,     // 6
  KEY_SOFT_CLIP_THRESHOLD,  // 7
  KEY_GAIN_PILOT,           // 8
  KEY_FREQ_DERIVATION,      // 9
  KEY_BUFFER_GAIN,          // 10
  KEY_RDS_FREQ_DEV,         // 11
  KEY_MAIN_SCREEN           // 12
};

/*
  The following table represents an "array" of default values (default position) for each transmitter parameter. 
  Thus, by default, the position ("key") points to the table position that corresponds to the parameter to be 
  configured for the transmitter. Complex? Yes, a bit. But this helps to write less code in C/C++ in case more 
  parameters are added to the system. See KeyValue datatype above.
*/
KeyValue keyValue[] = {
  { 0, NULL },                     // KEY_FREQUENCIA
  { 0, NULL },                     // KEY_POWER
  { 0, tabMonoStereo },            // KEY_MONO_ESTEREO
  { 1, tabPreEmphasis },           // KEY_PRE_EMPHASIS
  { 0, tabRDS },                   // KEY_RDS
  { 2, tabImpedance },             // KEY_INPEDANCE
  { 0, tabTxSoftClipEnable },      // KEY_SOFT_CLIP_ENABLE
  { 1, tabTxSoftClipThreshold },   // KEY_SOFT_CLIP_THRESHOLD
  { 2, tabGainTxPilot },           // KEY_GAIN_PILOT
  { 2, tabTxFrequencyDeviation },  // KEY_FREQ_DERIVATION
  { 1, tabTxBufferGain },          // KEY_BUFFER_GAIN
  { 2, txRdsFreqDev },             // KEY_RDS_FREQ_DEV
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
                     (char *)"FM Transmitters Enthusiasts    \r",
                     (char *)"QN8066 HOMEBREW FM TRANSMITTER \r" };

const uint8_t lastRdsPS = (sizeof(rdsPSmsg) / sizeof(rdsPSmsg[0])) - 1;
const uint8_t lastRdsRT = (sizeof(rdsRTmsg) / sizeof(rdsRTmsg[0])) - 1;

uint8_t idxRdsPS = 0;
uint8_t idxRdsRT = 0;

#define RDS_PS_REFRESH_TIME 5000
#define RDS_RT_REFRESH_TIME 15000

long rdsTimePS = millis();
long rdsTimeRT = millis();


// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  pinMode(PWM_PA, OUTPUT);  // Sets the Arduino PIN to operate with with PWM
  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);

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
  delay(2000);

  lcd.clear();

  checkQN8066('%');

  // Check the EEPROM content. If it contains valid data, read it (previous setup).
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllTransmitterInformation();
  } else {
    // Defult values
    txFrequency = 1069;
    pwmPowerDuty = 50;
    saveAllTransmitterInformation();
  }

  enablePWM(0);  // PWM disable

  tx.setup();
  tx.setTX(txFrequency);
  delay(100);

  // tx.resetAudioPeak();

  // Due to the architecture of the KIT, the PWM interferes with I2C communication.
  // Therefore, before changing the transmitter's configuration parameters, it must be disabled (Duty 0).

  // Sets the transmitter with the previous setup parameters
  tx.setTxInputImpedance(keyValue[KEY_INPEDANCE].value[keyValue[KEY_INPEDANCE].key].idx);  // 40Kohm
  tx.setTxPilotGain(keyValue[KEY_GAIN_PILOT].value[keyValue[KEY_GAIN_PILOT].key].idx);
  tx.setTxSoftClippingEnable(keyValue[KEY_SOFT_CLIP_ENABLE].value[keyValue[KEY_SOFT_CLIP_ENABLE].key].idx);
  tx.setTxSoftClipThreshold(keyValue[KEY_SOFT_CLIP_THRESHOLD].value[keyValue[KEY_SOFT_CLIP_THRESHOLD].key].idx);
  tx.setPreEmphasis(keyValue[KEY_PRE_EMPHASIS].value[keyValue[KEY_PRE_EMPHASIS].key].idx);
  tx.rdsTxEnable(keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx);
  tx.setTxMono(keyValue[KEY_MONO_ESTEREO].value[keyValue[KEY_MONO_ESTEREO].key].idx);
  tx.setTxInputBufferGain(keyValue[KEY_BUFFER_GAIN].value[keyValue[KEY_BUFFER_GAIN].key].idx);
  tx.rdsSetFrequencyDerivation(keyValue[KEY_RDS_FREQ_DEV].value[keyValue[KEY_RDS_FREQ_DEV].key].idx);

  showStatus(lcdPage);

  // Checking RDS setup
  if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
    tx.rdsInitTx(0, 0, 0, 5, 50, 6);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html)
    sendRDS();              // Control the RDS PS and RT messages with this function
  }

  enablePWM(pwmPowerDuty);  // It is about 1/5 of the max power. At 50 duty cycle, it is between 1 and 1,4 W
}


void checkQN8066(char c) {
  uint8_t count = 0;
  while (!tx.detectDevice() && (count < 20) ) {
    // tx.stopTransmitting();
    Wire.end();
    delay(1);
    Wire.begin();
    delay(2);  
    count++;
  }
  lcd.setCursor(15, 0);
  if (count > 0) {
    lcd.print(c); 
    tx.startTransmitting();
  } else {
    lcd.print(' '); 
  }
}

// Saves current transmitter setup
void saveAllTransmitterInformation() {
  // The update function/method writes data only if the current data is not equal to the stored data.
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, txFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 2, txFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 3, pwmPowerDuty);
  EEPROM.update(eeprom_address + 4, keyValue[KEY_MONO_ESTEREO].key);
  EEPROM.update(eeprom_address + 5, keyValue[KEY_PRE_EMPHASIS].key);
  EEPROM.update(eeprom_address + 6, keyValue[KEY_RDS].key);
  EEPROM.update(eeprom_address + 7, keyValue[KEY_INPEDANCE].key);
  EEPROM.update(eeprom_address + 8, keyValue[KEY_SOFT_CLIP_ENABLE].key);
  EEPROM.update(eeprom_address + 9, keyValue[KEY_SOFT_CLIP_THRESHOLD].key);
  EEPROM.update(eeprom_address + 10, keyValue[KEY_GAIN_PILOT].key);
  EEPROM.update(eeprom_address + 11, keyValue[KEY_FREQ_DERIVATION].key);
  EEPROM.update(eeprom_address + 12, keyValue[KEY_BUFFER_GAIN].key);
  EEPROM.update(eeprom_address + 13, keyValue[KEY_RDS_FREQ_DEV].key);
}
// Read the previous transmitter setup
void readAllTransmitterInformation() {
  txFrequency = EEPROM.read(eeprom_address + 1) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 2);
  pwmPowerDuty = EEPROM.read(eeprom_address + 3);
  keyValue[KEY_MONO_ESTEREO].key = EEPROM.read(eeprom_address + 4);
  keyValue[KEY_PRE_EMPHASIS].key = EEPROM.read(eeprom_address + 5);
  keyValue[KEY_RDS].key = EEPROM.read(eeprom_address + 6);
  keyValue[KEY_INPEDANCE].key = EEPROM.read(eeprom_address + 7);
  keyValue[KEY_SOFT_CLIP_ENABLE].key = EEPROM.read(eeprom_address + 8);
  keyValue[KEY_SOFT_CLIP_THRESHOLD].key = EEPROM.read(eeprom_address + 9);
  keyValue[KEY_GAIN_PILOT].key = EEPROM.read(eeprom_address + 10);
  keyValue[KEY_FREQ_DERIVATION].key = EEPROM.read(eeprom_address + 11);
  keyValue[KEY_BUFFER_GAIN].key = EEPROM.read(eeprom_address + 12);
  keyValue[KEY_RDS_FREQ_DEV].key = EEPROM.read(eeprom_address + 13);
}

// Enable or disable PWM duty cycle
void enablePWM(uint8_t value) {
    if ( value == 0) { 
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
  enablePWM(0);  // PWM duty cycle disabled
  tx.setTX(txFrequency = freq);
  enablePWM(pwmPowerDuty);  // PWM duty cycle anable
  showFrequency();
}
// Shows the first message after turn the transmitter on
void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1000);
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
  // uint16_t currentPower = (uint16_t)(pwmPowerDuty * 7 / 255);
  uint16_t currentPower = (uint16_t)(pwmPowerDuty * 100 / 255);
  sprintf(strPower, "%d%%  ", currentPower);
  lcd.setCursor(0, 1);
  lcd.print(strPower);
}
// Shows the general current transmitter status
void showStatus(uint8_t page) {
  char strFrequency[9];
  char str[20];

  lcd.clear();
  checkQN8066('$');
  if (page == 0) {
    tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
    lcd.setCursor(0, 0);
    lcd.print(strFrequency);
    lcd.print("MHz");
    lcd.setCursor(9, 0);
    lcd.print(keyValue[KEY_MONO_ESTEREO].value[keyValue[KEY_MONO_ESTEREO].key].desc);  // Mono Stereo
    lcd.setCursor(0, 1);
    lcd.print(tx.getAudioPeakValue());
    lcd.print("mV");
    lcd.setCursor(9, 1);
    sprintf(str, "PA:%d%%", pwmPowerDuty * 100 / 255);
    lcd.print(str);
    tx.resetAudioPeak();
  } else if (page == 1) {
    lcd.setCursor(0, 0);
    sprintf(str, "FSM: %d", tx.getFsmStateCode());
    lcd.print(str);
    sprintf(str, "RIN:%s", keyValue[KEY_INPEDANCE].value[keyValue[KEY_INPEDANCE].key].desc);
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
  } else {
    sprintf(str, "%s PTY:%2d", tx.rdsGetPS(), tx.rdsGetPTY());
    lcd.setCursor(0, 0);
    lcd.print(str);
    lcd.setCursor(0, 1);
    sprintf(str, "RDS ERR: %d", tx.rdsGetError());
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
  uint8_t browse;
  do {
    browse = checkButton();
    if (browse == BT_DOWN_PRESSED)  // Down/Left pressed
      return -1;
    else if (browse == BT_UP_PRESSED)  // Up/Right pressed
      return 1;
  } while (browse == BT_NO_PRESSED);
  return 0;
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
  while (key != 0) {
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
  menuLevel = 0;
}
// // Processes the change to a new power (PWM duty cycle)
void doPower() {
  showPower();
  int8_t key = browseParameter();
  while (key != 0) {
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
  while (key != 0) {
    if (key == 1) {
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
  menuLevel = 0;
}
// // Processes the current menu option selected
uint8_t doMenu(uint8_t idxMenu) {
  enablePWM(0);  // The PWM seems to interfere with the communication with the QN8066.
  delay(PUSH_MIN_DELAY);
  switch (idxMenu) {
    case 0:
      lcd.setCursor(9, 1);
      lcd.print("<<");  // it just indicates the edit mode
      doFrequency();
      break;
    case 1:
      lcd.setCursor(9, 1);
      lcd.print("<<");  // it just indicates the edit mode
      doPower();
      break;
    case 2:
      runAction([](uint8_t value) {tx.setTxMono(value);}, &keyValue[idxMenu], 1, 0, 1);
      break;
    case 3:
      runAction([](uint8_t value) {tx.setPreEmphasis(value);},&keyValue[idxMenu], 1, 0, 1);
      break;
    case 4:
      runAction([](uint8_t value) {tx.rdsTxEnable(value);}, &keyValue[idxMenu], 1, 0, 1);
      break;
    case 5:
      runAction([](uint8_t value) {tx.setTxInputImpedance(value);}, &keyValue[idxMenu], 1, 0, 3);
      break;
    case 6:
      runAction([](uint8_t value) {tx.setTxSoftClippingEnable(value);}, &keyValue[idxMenu], 1, 0, 1);
      break;
    case 7:
      runAction([](uint8_t value) {tx.setTxSoftClipThreshold(value);},&keyValue[idxMenu], 1, 0, 3);
      break;
    case 8:
      runAction([](uint8_t value) {tx.setTxPilotGain(value);}, &keyValue[idxMenu], 1, 0, 3);
      break;
    case 9:
      runAction([](uint8_t value) {tx.setTxFrequencyDerivation(value);},&keyValue[idxMenu], 1, 0, 5);
      break;
    case 10:
      runAction([](uint8_t value) {tx.setTxInputBufferGain(value);}, &keyValue[idxMenu], 1, 0, 5);
      break;
    case 11:
      runAction([](uint8_t value) {tx.rdsSetFrequencyDerivation(value);}, &keyValue[idxMenu], 1, 0, 5);
      break;
    case 12:
      enablePWM(pwmPowerDuty);  // Turn the PWM on again.
      return 0;
      break;
    default:
      break;
  }
  enablePWM(pwmPowerDuty);          // Turn the PWM on again.
  saveAllTransmitterInformation();  // Saves the current modified data to the EEPROM
  return 1;
}

/**
   Here are some recommended configurations for testing the RDS features
   1: Stereo = On
   2: Pre-emphasis = 75 us
   3: RDS = Enable
   4: Inpedance = 20K
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
    tx.rdsSendRTMessage(rdsRTmsg[idxRdsRT]);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    idxRdsRT++;
    rdsTimeRT = millis();
  }
}

/*
 Returns:
          7 if no key is pressed. -  111 - BT_NO_PRESSED
          6 if BT_DOWN is pressed -  110 - BT_DOWN_PRESSED 
          5 if BT_UP is pressed   -  101 - BT_UP_PRESSED 
          3 if BT_MENU is pressed -  011 - BT_MENU_PRESSED
  // TODO - Debounce process 
*/
int8_t checkButton() {
  uint8_t button;
  for (uint8_t i = 0; i < 5; i++) {
    // Please... check it out later
    button = digitalRead(BT_MENU) << 2 | digitalRead(BT_DOWN) << 1 | digitalRead(BT_UP);
    delay(30);
  }
  
  if ( (millis() - i2c_monitor_time) > 250) {
    checkQN8066('#');
    i2c_monitor_time = millis();
  }

  return button;
}

// Main loop
void loop() {
  int8_t key;
  if (menuLevel == 0) {
    showStatus(lcdPage);
    while ((key = checkButton()) == BT_NO_PRESSED) {

      // RDS UNDER CONSTRUCTION...
      if (keyValue[KEY_RDS].value[keyValue[KEY_RDS].key].idx == 1) {
        sendRDS();
      }

      // Refresh Status
      if ((millis() - showStatusTime) > STATUS_REFRESH_TIME) {
        showStatus(lcdPage);
        showStatusTime = millis();
      }

    }
    if (key == BT_DOWN_PRESSED) {  // Down Pressed
      lcdPage--;
      if (lcdPage < 0) lcdPage = 3;
      showStatus(lcdPage);
    } else if (key == BT_UP_PRESSED) {  // Up Pressed
      lcdPage++;
      if (lcdPage > 3) lcdPage = 0;
      showStatus(lcdPage);
    } else {  // Menu Pressed
      menuLevel = 1;
    }
  } else if (menuLevel == 1) {
    showMenu(menuIdx);
    key = browseParameter();
    while (key != 0) {
      if (key == -1) {
        if (menuIdx == 0)
          menuIdx = lastMenu;
        else
          menuIdx--;
      } else if (key == 1) {
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
