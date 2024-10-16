/*
  Attiny24, OLED and QN8066 wireup  
  | Device pin      | Attiny85 REF pin | Physical pin  | 
  | ----------------| -----------------| ------------- | 
  | PUSH BUTTON     |                  |               |
  | UP              |     PB1          |     6         | 
  | DOWN            |     PB4          |     3         |
  | MENU            |     PB3          |     2         | 
  |                 |                  |               |
  | QN8066 & OLED   |                  |               | 
  | SDIO / SDA      |     SDA / PA6    |     7         |
  | SCLK / CLK      |     SCL / PA4    |     9         |

  Compiling and uploading: 
  1) Install the ATtiny Core in Arduino IDE - Insert the URL http://drazzy.com/package_drazzy.com_index.json on board manager. 
                                              To do that, go to Preferences, enter the above URL in "Additional Boards Manager URLs. 
  2) Setup: Chip: Clock Source: 1MHz (Internal); LTO Enabled; millis() / macros() Enabled; 
  ATTENTION: if you select Clock source 8 MHz, for some reason, the system will work very slow. Maybe a bug. 
  
  See documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html 
  By Ricardo Lima Caratti, 2024.
*/

#include <QN8066.h>

uint16_t currentFrequency = 1069; // 106.9 MHz is the default frequency

QN8066 tx;

void  inline showStatus() {
  char strFrequency[7];
  tx.convertToChar(currentFrequency, strFrequency, 4, 3, ',');  // Convert the selected frequency a array of char
}

void setup() {
  tx.setup(1000,false,true);
  tx.setTX(currentFrequency);
  tx.rdsTxEnable(true);
  showStatus();
}


void loop() {
  tx.rdsSendPS((char *) "QN8066TX");
  delay(30000);
}
