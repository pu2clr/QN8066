#include "MENU_LCD.h"

MENU_LCD::MENU_LCD(){

}

MENU_LCD::MENU_LCD(uint8_t menu_pin, uint8_t up_pin, uint8_t down_pin, LiquidCrystal *lcd) {
  pinMode(this->pin_menu = menu_pin, INPUT_PULLUP);
  pinMode(this->pin_up = up_pin, INPUT_PULLUP);
  pinMode(this->pin_down = down_pin, INPUT_PULLUP);
}

void MENU_LCD::addItem(char *itemDesc, TableValue *tab, uint8_t currentKey, uint8_t maxKeyValue ) {
  if (this->idx < MAX_MENU_ITEM) { 
    this->item[this->idx] = itemDesc;
    this->keyDesc[this->idx] = tab;
    this->currentKey[this->idx] = currentKey;
    this->maxKey[this->idx] = maxKeyValue; 
    this->idx++;
  }
}

char * MENU_LCD::getItem(uint8_t idxItem) {
  return item[idxItem];
}

char* MENU_LCD::getParameterDesc(int8_t idxItem) { 
  return (char *) keyDesc[idxItem][currentKey[idxItem]].desc;
}

uint8_t MENU_LCD::getParameterValue(int8_t idxItem) { 
  return  keyDesc[idxItem][currentKey[idxItem]].value;
}

int8_t MENU_LCD::browseParameter() {
  do {
    delay(PUSH_MIN_DELAY);
    uint8_t browse = (digitalRead(this->pin_up) << 1) | (digitalRead(this->pin_down));
    if (browse == 1)  // Down/Left pressed
      return -1;
    else if (browse == 2)  // Up/Right pressed
      return 1;
    delay(PUSH_MIN_DELAY);
  } while (digitalRead(this->pin_menu) == HIGH);
  return 0;
}

/**
 * @brief Runs the action menu to modify the given parameter.
 * @details This function generalizes the menu functions that modify the transmitter parameter values.
 * @details For this purpose, it receives as parameters pointers to functions that will be executed (depending on what needs to be modified).
 * @param actionFunc      Pointer to the function that modifies the parameter.
 * @param idxMenuItem     Menu item
 * @param currentKey      CurrentKey value of the parameter correspondent to Menu Item 
 * @param col             column to be printed
 * @param lin             line to be preinted
 * @see C/C++: Syntax for declaring function pointers; Assigning functions to function pointers; Calling functions through function pointers.
 * @see C/C++: Passing function pointers as arguments to other functions; Understanding how to use function pointers for callback mechanisms.
 * @see   C++: Capturing variables in lambdas and their usage as function pointers; Understanding lambda expressions in C++ and how they relate to function pointers  
 */
void MENU_LCD::runMenuKeyValue(void (*actionFunc)(uint8_t), int8_t idxMenuItem, uint8_t currentKey, uint8_t col, uint8_t lin) {  
  lcd->clear();
  lcd->setCursor(col,lin);
  lcd->print(this->getItem(idxMenuItem)); 
  lcd->setCursor(col,lin+1);
  lcd->print ( this->keyDesc[idxMenuItem][this->currentKey[idxMenuItem]].desc );  
  int8_t key = this->browseParameter();
  while (key != 0) {
    if  ( key ==  1) { 
        if ( this->currentKey[idxMenuItem] == this->maxKey[idxMenuItem]) 
           this->currentKey[idxMenuItem] = 0;
        else 
           this->currentKey[idxMenuItem]++;  
    } else {
        if (this->currentKey[idxMenuItem] == 0) 
           this->currentKey[idxMenuItem] = this->maxKey[idxMenuItem];
        else 
           this->currentKey[idxMenuItem]--;  
    }
    actionFunc(this->getParameterValue(idxMenuItem));
    lcd->setCursor(col,lin+1);
    lcd->print ( this->keyDesc[idxMenuItem][this->currentKey[idxMenuItem]].desc );  
    key = this->browseParameter();
  }
}

 void MENU_LCD::runMenuKeyValue(void (*showFunc)(void), void (*actionFunc)(uint16_t),  int8_t idxMenuItem, uint16_t *value, uint16_t minValue, uint16_t maxValue, uint16_t step) {
  showFunc();
  int8_t key = this->browseParameter();
  while (key != 0) {
    if (key == 1) {
      if (*value < maxValue )
        *value += step;
      else   
        *value = minValue;
    } else if (key == -1) {
      if (*value > minValue )
        *value -= step;
      else   
        *value = maxValue;      
    }
    actionFunc(*value);
    showFunc();
    key = this->browseParameter();
  }
 }


  void MENU_LCD::showFirstItem() {
       this->idx = 0; 
       lcd->setCursor(0,0);
       lcd->print(this->item[this->idx]); 
       lcd->setCursor(0,1);
       lcd->print(this->keyDesc[this->idx][this->currentKey[this->idx]].desc);  
  } 

  void MENU_LCD::showLastItem() {
        this->idx = MAX_MENU_ITEM - 1; 
       lcd->setCursor(0,0);
       lcd->print(this->item[this->idx]); 
       lcd->setCursor(0,1);
       lcd->print(this->keyDesc[this->idx][this->currentKey[this->idx]].desc);  
  } 

  void MENU_LCD::showNextItem() {
    this->idx++; 
    if (this->idx >= MAX_MENU_ITEM) this->idx = 0; 
       lcd->setCursor(0,0);
       lcd->print(this->item[this->idx]); 
       lcd->setCursor(0,1);
       lcd->print(this->keyDesc[this->idx][this->currentKey[this->idx]].desc);
    }

  void MENU_LCD::showPreviusItem() {
    if (this->idx == 0) 
       this->idx = MAX_MENU_ITEM - 1; 
    else {
       this->idx--;   
       lcd->setCursor(0,0);
       lcd->print(this->item[this->idx]); 
       lcd->setCursor(0,1);
       lcd->print(this->keyDesc[this->idx][this->currentKey[this->idx]].desc);
    }
  } 

uint8_t MENU_LCD::browse() 
{
    this->showFirstItem();
    uint8_t key = this->browseParameter();
    while (key != 0) {
      if (key == -1) 
        showPreviusItem();
      else if (key == 1) 
        showNextItem();     
      key = browseParameter();
    }
    return this->idx;
}


