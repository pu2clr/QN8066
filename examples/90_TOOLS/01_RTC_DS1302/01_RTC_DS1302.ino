/**
 * @file 01_RTC_DS1302.ino
 * @author Ricardo Lima Caratti (pu2clr@gmail.com)
 * @brief This sketch is a tool for configuring the DS1302-based Real-Time Clock (RTC). Please follow the comments in the sketch for guidance.
 * @details Install the library Ds1302 (A C/C++ library to use DS1302 RTC chip.) by Rafa Couto <caligari@treboada.net>  
 * @date 2024-08-29
 */

#include <Ds1302.h>

#define CLK_PIN     14      // Using the D14/A0 for clock
#define DATA_PIN    15      // Using the D15/A1 for data
#define RESET_PIN   16      // Using the D16/A2 for reset the DS1302 device

Ds1302 rtc(RESET_PIN, CLK_PIN, DATA_PIN);

Ds1302::DateTime dt;

void setup() {
  Serial.begin(9600); //Inicialização da comunicação serial
  rtc.init(); //Inicializa o rtc

  // To set the RTC, uncomment this block of lines, compile and upload the sketch to the Arduino. 
  // Once the clock is set, comment out the lines again, compile and upload the sketch.  
  
  dt.year = 24;
  dt.month = 9;
  dt.day = 12;
  dt.dow = 1;
  dt.hour = 15;
  dt.minute = 26;
  dt.second = 50;
  rtc.setDateTime(&dt);
  dt.dow = 0;
  rtc.setDateTime(&dt);

}

char str[80];
void loop() {
  //Obtém as informações de tempo atual
  rtc.getDateTime(&dt);
  // if (dt.dow) {
    sprintf(str,"\n=> %2.2d/%2.2d/20%2.2d - %d:%d:%d", dt.day, dt.month, dt.year, dt.hour, dt.minute, dt.second);
    Serial.print(str);
  // }

  delay(1000);
}