/*
  UNDER CONSTRUCTION... 
  DIY KIT 5~7W QN8066 FM TRANSMITTER controlled by Arduino Nano
  This sketch uses an Arduino Nano with LCD16X02.

  ABOUT THE ATMEGA328 EEPROM and saving the receiver current information
  ATMEL says the lifetime of an EEPROM memory position is about 100,000 writes.

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

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

// Enconder PINs

#define BT_MENU 8
#define BT_UP 10
#define BT_DOWN 11
#define PWM_PA 9

#define STEP_FREQ 1;

#define PUSH_MIN_DELAY 200

#define TIME_PAGE 5000

uint32_t timePage = millis();
uint8_t lcdPage = 0;

uint8_t menuLevel = 0;

const uint8_t app_id = 86;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;

// Menu
const char *menu[] = { "Frequency", "Power", "Stereo/Mono", "Pre-emphasis", "RDS", "Inpedance","Sft Clip. Enable",  "Sft Clip. Thres.",  "Gain Pilot", "Freq. Deriv.", "Buffer gain", "Main Screen" };
int8_t menuIdx = 0;
const int lastMenu = 11;

uint8_t frequencyStep = 100;
// The PWM duty can be set from 25 to 255 where 255 is the max power (7W) .
// So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPowerDuty = 50;  // Initial power/duty.
uint8_t pwmDutyStep = 25;

// Tables and parameter values

typedef struct
{
  uint8_t idx;       // Value of the parameter
  const char *desc;  // Description
} TableValue;

int8_t idxImpedance = 2;
TableValue tabImpedance[] = {
  { 0, "10K" },  // 0
  { 1, "20K" },  // 1
  { 2, "40K" },  // 2
  { 3, "80K" }   // 3
};

int8_t idxGainTxPilot = 2;
TableValue tabGainTxPilot[] = {
  { 7, "7% * 75KHz" },   // 0
  { 8, "8% * 75KHz" },   // 1
  { 9, "9% * 75KHz" },   // 2
  { 10, "10% * 75KHz" }  // 3
};

int8_t idxTxSoftClipEnable = 1;
TableValue tabTxSoftClipEnable[] = {
  { 0, "Disable " },     // 0
  { 1, "Enable  " }      // 1
};

int8_t idxTxSoftClipThreshold = 0;
TableValue tabTxSoftClipThreshold[] = {
  { 0, "12'd2051 (3dB" },     // 0
  { 1, "12'd1725 (4.5dB)" },  // 1
  { 2, "12'd1452 (6dB)" },    // 2
  { 3, "12'd1028 (9dB)" }     // 3
};

int8_t idxTxFrequencyDeviation = 2;
TableValue tabTxFrequencyDeviation[] = {
  {  60, " 41,40kHz"},  // 0
  {  87, " 60,03kHz"},  // 1
  { 108, " 74,52kHz"},  // 2
  { 120, " 92,80kHz"},  // 3
  { 140, " 96,60kHz"},  // 4
  { 160, "110,40kHz"}   // 5
};

int8_t idxTxBufferGain = 1;
TableValue tabTxBufferGain[] = {
  {  0, "3dB"},  // 0
  {  1, "6dB"},  // 1
  {  2, "9dB"},  // 2
  {  3, "12dB"},  // 3
  {  4, "15dB"},  // 4
  {  5, "18dB"}   // 5
};

int8_t idxPreEmphasis = 1;
TableValue tabPreEmphasis[] = {
  { 0, "50 us" },   // 0
  { 1, "75 us" }    // 1
};

int8_t idxRDS = 0;
TableValue tabRDS[] = {
  { 0, "Disable" },     // 0
  { 1, "Enable " }      // 1
};

int8_t idxStereoMono = 0;
TableValue tabMonoStereo [] = {
  { 0, "Stereo" },     // 0
  { 1, "Mono  " }      // 1 - See QN8066 data sheet
};

uint16_t txFrequency = 1069;  // Default frequency is 106.9 MHz

uint8_t inputInpedance = 2;  // Default 20 KOhms
bool bShow = false;

// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {

  pinMode(PWM_PA, OUTPUT);  // Sets the Arduino PIN to operate with with PWM

  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);

  lcd.begin(16, 2);


  // If you want to reset the eeprom, keep the VOLUME_UP button pressed during statup
  if (digitalRead(BT_MENU) == LOW) {
    lcd.clear();
    EEPROM.write(eeprom_address, 0);
    lcd.setCursor(0, 0);
    lcd.print("RESET TO DEFAULT");
    delay(4000);
    lcd.clear();
  }

  showSplash();
  delay(3000);

  lcd.clear();

  if (!tx.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1) 
      ;
  }

  // Checking the EEPROM content
  if (EEPROM.read(eeprom_address) == app_id) {  
    readAllTransmitterInformation();
  } else { 
    // Defult values
    txFrequency = 1069;
    pwmPowerDuty = 50;
    idxImpedance = 2; // 40Kohm
    idxGainTxPilot = 2;
    idxTxSoftClipEnable = 1;
    idxTxSoftClipThreshold = 0;
    idxPreEmphasis = 1;
    idxRDS = 0;
    idxStereoMono = 0; // Sets to stereo mode
    idxTxBufferGain = 1;

    saveAllTransmitterInformation();
  }

  tx.setup();
  tx.setTX(txFrequency);

  analogWrite(PWM_PA, 0); // Disable PWM

  tx.setTxInputImpedance(idxImpedance); // 40Kohm
  tx.setTxPilotGain(idxGainTxPilot);
  tx.setTxSoftClippingEnable(idxTxSoftClipEnable);
  tx.setTxSoftClipThreshold(idxTxSoftClipThreshold);
  tx.setPreEmphasis(idxPreEmphasis);
  tx.rdsTxEnable(idxRDS);
  tx.setTxMono(idxStereoMono); 
  tx.setTxInputBufferGain(idxTxBufferGain);
  showStatus(lcdPage);
  lcd.clear();
  delay(500);
  analogWrite(PWM_PA, pwmPowerDuty);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void saveAllTransmitterInformation() {
  // The update function/method writes data only if the current data is not equal to the stored data.
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, inputInpedance);      // stores the current inputInpedance
  EEPROM.update(eeprom_address + 2, txFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 3, txFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 4, idxRDS);
  EEPROM.update(eeprom_address + 5, idxStereoMono);
  EEPROM.update(eeprom_address + 6, pwmPowerDuty);

  EEPROM.update(eeprom_address + 7, idxStereoMono);
  EEPROM.update(eeprom_address + 8, idxPreEmphasis);
  EEPROM.update(eeprom_address + 9, idxRDS);
  EEPROM.update(eeprom_address +10, idxImpedance);
  EEPROM.update(eeprom_address +11, idxTxSoftClipEnable);
  EEPROM.update(eeprom_address +12, idxTxSoftClipThreshold);
  EEPROM.update(eeprom_address +13, idxGainTxPilot);
  EEPROM.update(eeprom_address +14, idxTxFrequencyDeviation);
  EEPROM.update(eeprom_address +15, idxTxBufferGain);
}


void readAllTransmitterInformation() {
  inputInpedance = EEPROM.read(eeprom_address + 1);
  txFrequency = EEPROM.read(eeprom_address + 2) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 3);
  idxRDS = EEPROM.read(eeprom_address + 4);
  idxStereoMono = EEPROM.read(eeprom_address + 5);
  pwmPowerDuty = EEPROM.read(eeprom_address + 6);

  idxStereoMono = EEPROM.read(eeprom_address + 7);
  idxPreEmphasis = EEPROM.read(eeprom_address + 8);
  idxRDS = EEPROM.read(eeprom_address + 9);
  idxImpedance = EEPROM.read(eeprom_address +10);
  idxTxSoftClipEnable = EEPROM.read(eeprom_address +11);
  idxTxSoftClipThreshold = EEPROM.read(eeprom_address +12);
  idxGainTxPilot = EEPROM.read(eeprom_address +13);
  idxTxFrequencyDeviation = EEPROM.read(eeprom_address +14);
  idxTxBufferGain = EEPROM.read(eeprom_address +15);

}

void enablePWM(uint8_t value) {
  analogWrite(PWM_PA, value);  // Turn PA off
  delay(200);
}

void switchTxFrequency(uint16_t freq) {
  enablePWM(0); // Duty - PWM disabled
  tx.setTX(txFrequency = freq);
  enablePWM(pwmPowerDuty);
  showFrequency();
}

void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-QN8066");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1000);
}

void showFrequency() {
  char strFrequency[7];
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 1);
  lcd.print(strFrequency);
  lcd.display();
}

void showPower() {
  char strPower[7];
  // uint16_t currentPower = (uint16_t)(pwmPowerDuty * 7 / 255);
  uint16_t currentPower = (uint16_t)(pwmPowerDuty * 100 / 255) ;
  sprintf(strPower, "%d%%  ", currentPower);
  lcd.setCursor(0, 1);
  lcd.print(strPower);
}

void showStatus(uint8_t page) {
  char strFrequency[7];
  char str[20];

  lcd.clear();
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 0);
  lcd.print(strFrequency);
  lcd.print("MHz");

  if ( page == 0) { 
     lcd.setCursor(10, 0);
     lcd.print(  tabMonoStereo[idxStereoMono].desc );
     lcd.setCursor(0, 1);
     lcd.print(tx.getAudioPeakValue());
     lcd.print("mV");
     lcd.setCursor(9, 1);
     sprintf(str,"PA:%d%%", pwmPowerDuty * 100 / 255 );
     lcd.print(str);
  }
  else if (page == 1) {     
      sprintf(str,"RIN:%s", tabImpedance[idxImpedance].desc);   
      lcd.setCursor(9, 0);
      lcd.print(str);
      lcd.setCursor(0, 1);
      sprintf(str,"DEV.: %s", tabTxFrequencyDeviation[idxTxFrequencyDeviation].desc);  
      lcd.print(str);

  }
  else {
      sprintf(str,"BG:%s", tabTxBufferGain[idxTxBufferGain].desc);   
      lcd.setCursor(9, 0);
      lcd.print(str);
      lcd.setCursor(0, 1);
      sprintf(str,"PIL.:%s", tabGainTxPilot[idxGainTxPilot].desc);  
      lcd.print(str);
   }   
    
  lcd.display();
}

void showParameter(char *desc) {
  lcd.setCursor(0,1);
  lcd.print(desc); 
}

int8_t browseParameter() {
  do {
    delay(PUSH_MIN_DELAY);
    uint8_t browse = (digitalRead(BT_UP) << 1) | (digitalRead(BT_DOWN));
    if (browse == 1)  // Down/Left pressed
      return -1;
    else if (browse == 2)  // Up/Right pressed
      return 1;
    delay(PUSH_MIN_DELAY);
  } while (digitalRead(BT_MENU) == HIGH);
  return 0;
}


void showMenu(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menu[idx]);
}

void doFrequency() {
  showFrequency();
  int8_t key = browseParameter();
  while (key != 0) {
    if (key == -1) {
      txFrequency -= STEP_FREQ;
    } else if (key == 1) {
      txFrequency += STEP_FREQ;
    }
    switchTxFrequency(txFrequency);
    showFrequency();
    key = browseParameter();
  }
  menuLevel = 0;
}

void doPower() {
  showPower();
  int8_t key = browseParameter();
  while (key != 0) {
    if (key == -1) {
      if ( pwmPowerDuty >=25 )
        pwmPowerDuty -= pwmDutyStep; 
      else 
        pwmPowerDuty = 0;       
    } else if (key == 1) {
      if (pwmPowerDuty <= 225 )
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
 * @param tab         Table (see TableValue) that contains the set of valid parameters.
 * @param idx         Pointer to the index variable that indicates the table position.
 * @param step        Step (increment) used for the parameter.
 * @param min         Minimum valid value.
 * @param max         Maximum valid value.
 * @see C/C++: Syntax for declaring function pointers; Assigning functions to function pointers; Calling functions through function pointers.
 * @see C/C++: Passing function pointers as arguments to other functions; Understanding how to use function pointers for callback mechanisms.
 * @see   C++: Capturing variables in lambdas and their usage as function pointers; Understanding lambda expressions in C++ and how they relate to function pointers  
 */
void runAction(void (*actionFunc)(uint8_t), TableValue *tab, int8_t *idx, uint8_t step,  uint8_t min, uint8_t max ) {  
  showParameter((char *) tab[*idx].desc);
  int8_t key = browseParameter();
  while (key != 0) {
    if  ( key ==  1) { 
        if ( *idx == max) 
           *idx = min;
        else 
           *idx = *idx + step;  
    } else {
        if (*idx == min) 
           *idx = max;
        else 
           *idx = *idx - step;  
    }
    actionFunc(tab[*idx].idx);
    showParameter((char *) tab[*idx].desc);
    key = browseParameter();
  }
  menuLevel = 0;    
}



uint8_t doMenu(uint8_t idxMenu) {

// It is necessary to turn off the PWM to change parameters.
// The PWM seems to interfere with the communication with the QN8066.

  analogWrite(PWM_PA, 0);  // Turn PA off
  delay(200);

  switch (idxMenu) {
    case 0:
      doFrequency();
      break;
    case 1:
      doPower();
      break;
    case 2:
      runAction([&tx](uint8_t value) { tx.setTxMono(value); }, tabMonoStereo, & idxStereoMono, 1, 0, 1);
      break;
    case 3:
      runAction([&tx](uint8_t value) { tx.setPreEmphasis(value); }, tabPreEmphasis, & idxPreEmphasis, 1, 0, 1);
      break;
    case 4:
      runAction([&tx](uint8_t value) { tx.rdsTxEnable(value); }, tabRDS, & idxRDS, 1, 0, 1);
      break;
    case 5:
      runAction([&tx](uint8_t value) { tx.setTxInputImpedance(value); }, tabImpedance, & idxImpedance, 1, 0, 3);
      break;
    case 6:
      runAction([&tx](uint8_t value) { tx.setTxSoftClippingEnable(value); }, tabTxSoftClipEnable, & idxTxSoftClipEnable, 1, 0, 1);
      break;
    case 7:
      runAction([&tx](uint8_t value) { tx.setTxSoftClipThreshold(value); }, tabTxSoftClipThreshold, & idxTxSoftClipThreshold, 1, 0, 3);
      break;
    case 8:
      runAction([&tx](uint8_t value) { tx.setTxPilotGain(value); }, tabGainTxPilot, & idxGainTxPilot, 1, 0, 3);
      break;
    case 9:
      runAction([&tx](uint8_t value) { tx.setTxFrequencyDerivation(value); }, tabTxFrequencyDeviation, & idxTxFrequencyDeviation, 1, 0, 5);
      break;  
    case 10:
      runAction([&tx](uint8_t value) { tx.setTxInputBufferGain(value); }, tabTxBufferGain, & idxTxBufferGain, 1, 0, 5);
      break;   
    case 11:
      return 0;       
    default:
      break;
  }

  // Turn the PWM on again. 
  delay(200);
  analogWrite(PWM_PA, pwmPowerDuty);  // Turn PA on

  saveAllTransmitterInformation();

  return 1;
}



void loop() {
  
  int8_t key;

  if (menuLevel == 0) {
    showStatus(lcdPage);
    while (digitalRead(BT_MENU) == HIGH) {
      if ( (millis() - timePage) > TIME_PAGE ) {
        if (lcdPage > 2) lcdPage = 0; 
        showStatus(lcdPage); 
        lcdPage++;
        timePage = millis();
      }
    }
    menuLevel = 1;
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

  delay(PUSH_MIN_DELAY);
}
