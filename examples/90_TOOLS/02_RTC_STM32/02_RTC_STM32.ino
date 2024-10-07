/*
This sketch sets the internal clock of the STM32F1X, considering that you are 
using an external passive 32.768kHz crystal and a 3V battery connected to the 
STM32 (VBAT) terminal/pin.

Please, Install 'STM32duino RTC' by STMicroeletronics before compiling this sketch

Ricardo Lima Caratti - Oct, 2024
*/

#include <STM32RTC.h> // Install 'STM32duino RTC' by STMicroeletronics  

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

void setup() {
  // Sets the current local Date and Time. - Change it for your current local time
  rtc.setClockSource(STM32RTC::LSE_CLOCK); // Using external Crystal (32.768 kHz)
  rtc.begin(); // initialize RTC 24H format

  rtc.setTime(15, 14, 0); // Sets Hour, Minute, Seconds
  rtc.setDate(0, 7, 10, 24); // Sets Week Day, Day, Month, Year    
}

void loop() {
    delay(1000);
}
