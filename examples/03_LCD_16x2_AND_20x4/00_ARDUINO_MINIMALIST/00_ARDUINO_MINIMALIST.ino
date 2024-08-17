/*
  This sketch presents a minimal version of an FM transmitter with RDS support 
  and displays some information on a 16x2 LCD. In this version, there are no 
  button or encoder controls for changing transmitter configuration parameters 
  at runtime. The idea behind this sketch is to serve as a starting point for 
  the user to develop their own interface.

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
  | --------------------------| --------------------------|---------------|



  Prototype documentation: https://pu2clr.github.io/RDA5807/
  PU2CLR RDA5807 API documentation: https://pu2clr.github.io/RDA5807/extras/apidoc/html/

  By PU2CLR, Ricardo,  Feb  2023.
*/

#include <QN8066.h>
#include <LiquidCrystal.h>

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

char *ps = "";


// TX board interface
QN8066 tx;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  lcd.begin(16, 2);
  showSplash();
  delay(2000);
  lcd.clear();

  if (!tx.detectDevice()) {
    lcd.setCursor(0, 0);
    lcd.print("No QN8066 found!");
    while (1) 
      ;
  }
  tx.setup();
  tx.setTX(1069); // Sets the trasmitter to 106.9 MHz

  tx.rdsTxEnable(true);
  tx.rdsInitTx(0, 0, 0, 5, 25, 6);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html)
  sendRDS();              // Control the RDS PS and RT messages with this function
  delay(1000);
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


void showStatus(uint8_t page) {
  char strFrequency[7];
  char str[20];

  lcd.clear();
  tx.convertToChar(txFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
  lcd.setCursor(0, 0);
  lcd.print(strFrequency);
  lcd.print("MHz");
 
    
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
