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

  Prototype documentation: https://pu2clr.github.io/RDA5807/
  PU2CLR RDA5807 API documentation: https://pu2clr.github.io/RDA5807/extras/apidoc/html/

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

#define BT_RESET 3
#define BT_MENU 8
#define BT_UP 10
#define BT_DOWN 11
#define PWM_PA 9

#define STEP_FREQ 1;

#define PUSH_MIN_DELAY 300


uint8_t menuLevel = 0;
int8_t upDown = 0;
int8_t lastUpDown = 0;

const uint8_t app_id = 43;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;
long storeTime = millis();

// Menu
const char *menu[] = { "Frequency", "Power", "Stereo/Mono", "Pre-emphasis", "RDS", "Inpedance", "TX Gain", "TX OFF" };
int8_t menuIdx = 0;
const int lastMenu = 7;
int8_t currentMenuCmd = -1;

uint8_t frequencyStep = 100;
// The PWM duty can be set from 25 to 255 where 255 is the max power (7W) .
// So, if the duty is 25 the power is about 0,7W =>  Power = duty * 7 / 255
uint8_t pwmPowerDuty = 50;  // Minimal power/duty.
uint8_t pwmDutyStep = 25;

// Tables and parameter values

// Datatype to deal with bandwidth on AM, SSB and FM in numerical order.
// Ordering by bandwidth values.
typedef struct
{
  uint8_t idx;       // SI473X device bandwidth index value
  const char *desc;  // bandwidth description
} TableValue;

int8_t idxImpedance = 1;
TableValue tabImpedance[] = {
  { 0, "10K" },  // 0
  { 1, "20K" },  // 1
  { 2, "40K" },  // 2
  { 3, "80K" }   // 3
};

int8_t idxGainTxPilot = 7;
TableValue tabGainTxPilot[] = {
  { 7, "7% * 75KHz" },   // 0
  { 8, "8% * 75KHz" },   // 1
  { 9, "9% * 75KHz" },   // 2
  { 10, "10% * 75KHz" }  // 3
};


int8_t idxTxSoftClip = 0;
TableValue tabTxSoftClip[] = {
  { 0, "12'd2051 (3dB" },     // 0
  { 1, "12'd1725 (4.5dB)" },  // 1
  { 2, "12'd1452 (6dB)" },    // 2
  { 3, "12'd1028 (9dB)" }     // 3
};

int8_t idxPreEmphasis = 0;
TableValue tabPreEmphasis[] = {
  { 50, "50 us" },   // 0
  { 75, "75 us" }    // 1
};


//
uint16_t txFrequency = 1069;  // Default frequency is 106.9 MHz
bool bRds = false;
bool bStereo = true;
uint8_t inputInpedance = 2;  // Default 20 KOhms

bool bShow = false;

// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {

  pinMode(PWM_PA, OUTPUT);  // Sets the Arduino PIN to operate with with PWM
  pinMode(BT_RESET, INPUT_PULLUP);
  pinMode(BT_MENU, INPUT_PULLUP);
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);

  lcd.begin(16, 2);

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
    // readAllReceiverInformation();
  }

  tx.setup();

  tx.setTX(txFrequency);
  showStatus();
  lcd.clear();
  showStatus();
  delay(500);
  analogWrite(PWM_PA, pwmPowerDuty);  // It is about 1/5 of the max power. It is between 1 and 1,4 W
}

void showDebug(char *msg, int value) {
  char str[40];
  sprintf(str, "%s-%d", msg, value);
  lcd.setCursor(7, 1);
  lcd.print(str);
}

void saveAllReceiverInformation() {
  // The update function/method writes data only if the current data is not equal to the stored data.
  EEPROM.update(eeprom_address, app_id);
  EEPROM.update(eeprom_address + 1, inputInpedance);      // stores the current inputInpedance
  EEPROM.update(eeprom_address + 2, txFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.update(eeprom_address + 3, txFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.update(eeprom_address + 4, (uint8_t)bRds);
  EEPROM.update(eeprom_address + 5, (uint8_t)bStereo);
  EEPROM.update(eeprom_address + 6, pwmPowerDuty);
  // TODO
}

void readAllReceiverInformation() {
  inputInpedance = EEPROM.read(eeprom_address + 1);
  txFrequency = EEPROM.read(eeprom_address + 2) << 8;
  txFrequency |= EEPROM.read(eeprom_address + 3);
  bRds = (bool)EEPROM.read(eeprom_address + 4);
  bStereo = (bool)EEPROM.read(eeprom_address + 5);
  pwmPowerDuty = EEPROM.read(eeprom_address + 6);
  // TODO
}


void switchTxFrequency(uint16_t freq) {
  analogWrite(PWM_PA, 0);  // Turn PA off
  delay(200);
  tx.setTX(txFrequency = freq);
  delay(200);
  analogWrite(PWM_PA, pwmPowerDuty);  // Turn PA on
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

/*
   Shows frequency information on Display
*/
void showFrequency() {
  char strFrequency[7];
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 1);
  lcd.print(strFrequency);
  lcd.display();
}

void showPower() {
  char strPower[7];
  uint16_t currentPower = (uint16_t)(pwmPowerDuty * 7 / 255);
  sprintf(strPower, "%d W", currentPower);
  lcd.setCursor(0, 1);
  lcd.print(strPower);
  lcd.display();
}

void showPreEmphasis() {
  lcd.setCursor(0, 1);
  lcd.print(tabPreEmphasis[idxPreEmphasis].desc);
  lcd.display();
}

/*
    Show some basic information on display
*/
void showStatus() {
  char strFrequency[7];

  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char

  lcd.setCursor(0, 0);
  lcd.print("TXing: ");
  lcd.print(strFrequency);
  lcd.print(" MHz");

  lcd.display();
}

void showStereoMono() {
  lcd.setCursor(0, 1);
  if (bStereo) {
    lcd.print("STEREO");
  } else {
    lcd.print("MONO  ");
  }
}

void showRds() {
  // TODO
}


void showImpedance(uint8_t idx) {
  lcd.setCursor(0,1);
  lcd.print(tabImpedance[idx].desc);
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



/*********************************************************

 *********************************************************/

void doStereo() {
  showStereoMono();
  int8_t key = browseParameter();
  while (key !=0) {
    bStereo = !bStereo;
    tx.setTxStereo(bStereo);
    showStereoMono();    
    key = browseParameter();
  }
  menuLevel = 0;
}


void doRds() {
  bRds = !bRds;
  showRds();
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

void doInpedance() {
  showImpedance(idxImpedance);
  int8_t key = browseParameter();
  while (key != 0) {
    if (key == -1) {
      idxImpedance = (idxImpedance > 0)? (idxImpedance - 1):3;         
    } else if (key == 1) {
      idxImpedance = (idxImpedance < 3)? (idxImpedance + 1):0; 
    }
    tx.setTxInputImpedance(tabImpedance[idxImpedance].idx); 
    showImpedance(idxImpedance);
    key = browseParameter();
  }
  menuLevel = 0;        
}

void doPreEmphasis() {
  showPreEmphasis();
  int8_t key = browseParameter();
  while (key != 0) {
    idxPreEmphasis = (key == 1)? 1:0;
    tx.setTxPreEmphasis(tabImpedance[idxPreEmphasis].idx); 
    showPreEmphasis();
    key = browseParameter();
  }
  menuLevel = 0;    

}

void doMenu(uint8_t idxMenu) {

  switch (idxMenu) {
    case 0:
      doFrequency();
      break;
    case 1:
      doPower();
      break;
    case 2:
      doStereo();
      break;
    case 3:
      doPreEmphasis();
      break;
    case 4:
      break;
    case 5:
      doInpedance();
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    default:
      break;
  }
  showStatus();
}


void loop() {
  // UNDER CONSTRUCTION...
  int8_t key;

  if (menuLevel == 0) {
    while (digitalRead(BT_MENU) == HIGH)
      ;
      menuIdx = 0;
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
    doMenu(menuIdx);
    menuLevel = 0;
  }


  delay(PUSH_MIN_DELAY);
}
