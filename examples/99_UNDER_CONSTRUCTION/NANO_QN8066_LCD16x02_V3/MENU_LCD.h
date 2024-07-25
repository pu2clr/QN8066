
#include <Arduino.h>
#include <LiquidCrystal.h>

#ifndef _MENU_LCD // Prevent this file from being compiled more than once
#define _MENU_LCD

#define PUSH_MIN_DELAY 200
#define MAX_MENU_ITEM 11

typedef struct
{
  uint8_t    value;
  const char *desc;  // Description
} TableValue;


class MENU_LCD
{
  private: 

  LiquidCrystal *lcd;  


  uint8_t pin_menu;
  uint8_t pin_up;
  uint8_t pin_down;

  uint8_t idx = 0;       // Value of the parameter
  char *item[MAX_MENU_ITEM];
  uint8_t maxKey[MAX_MENU_ITEM];
  uint8_t currentKey[MAX_MENU_ITEM];
  uint8_t menuLevel = 0;
  TableValue *keyDesc[MAX_MENU_ITEM];  // Description


  public:
  MENU_LCD();
  MENU_LCD(uint8_t menu_pin, uint8_t up_pin, uint8_t down_pin, LiquidCrystal *lcd);
  void addItem(char *itemDesc, TableValue *tab, uint8_t currentKey, uint8_t maxKeyValue); 
  char *getItem(uint8_t idx);
  char *getParameterDesc(int8_t idxItem);
  uint8_t getParameterValue(int8_t idxItem);
  void runMenuKeyValue(void (*actionFunc)(uint8_t), int8_t idxMenuItem, uint8_t currentKey, uint8_t col, uint8_t lin); 
  void runMenuKeyValue(void (*showFunc)(void), void (*actionFunc)(uint16_t),  int8_t idxMenuItem, uint16_t *value, uint16_t minValue, uint16_t maxValue, uint16_t step);
  int8_t browseParameter();
  inline uint8_t getCurrentMenuLevel() { return this->menuLevel;};
  inline void setCurrentMenuLevel(uint8_t level) {this->menuLevel = level;}; 
  inline void waitMenuAction() { while ( digitalRead(this->pin_menu) == HIGH) ; this->idx = 0;};
  void showNextItem();
  void showPreviusItem(); 
  void showFirstItem(); 
  void showLastItem();
  uint8_t browse();
};

#endif // _QN8066_H

