ECHO OFF
Rem This script uses the arduino-cli to compile the arduino sketches using command line (without Arduino IDE).
Rem It is very useful to check the library on multiple board types  after bug fixes and improvements. 
Rem It runs on a Windows CMD\Shell.
Rem Some compilation results (bin and hex files) will be stores in your Download directory (\Users\<username>\Downloads\hex)
Rem ATTENTION: be careful with --output-dir arduino-cli option. Before compiling, this option removes all the contents of the last level folder. 
Rem            For example: if you use "--output-dir %homepath%\Downloads", all the current content of the Downloads folder will be lost. 
Rem                         if you use "--output-dir Downloads\hex", all current content of the hex folder will be lost and the Downloads 
Rem                         content will be preserved. 
Rem  Ricardo Lima Caratti Mar 2024
ECHO ON 

echo "\n\n**** Arduino ATmega328 based board ***"
echo "---> A_MINIMALIST_TX"
echo "**** **** **** **** **** **** **** ***"

arduino-cli compile -b arduino:avr:nano .\01_TX_SERIAL_MONITOR\A_MINIMALIST_TX --output-dir %homepath%\Downloads\hex\atmega\A_MINIMALIST_TX
echo "---> A_TX"
arduino-cli compile -b arduino:avr:nano .\01_TX_SERIAL_MONITOR\A_TX --output-dir %homepath%\Downloads\hex\atmega\A_TX
echo "---> B_TX"
arduino-cli compile -b arduino:avr:nano .\01_TX_SERIAL_MONITOR\B_TX --output-dir %homepath%\Downloads\hex\atmega\B_TX
echo "---> TX_RDS1"
arduino-cli compile -b arduino:avr:nano .\02_TX_RDS\TX_RDS1 --output-dir %homepath%\Downloads\hex\atmega\TX_RDS1
echo "---> TX_RDS2"
arduino-cli compile -b arduino:avr:nano .\02_TX_RDS\TX_RDS2 --output-dir %homepath%\Downloads\hex\atmega\TX_RDS2

echo "\n\nLCD16x02 - Minimalist"   
echo "**** **** **** **** **** **** **** ***"
echo "---> 00_ARDUINO_MINIMALIST"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\00_ARDUINO_MINIMALIST --output-dir %homepath%\Downloads\hex\atmega\00_ARDUINO_MINIMALIST


echo "\n\nArduino Atmega328 LCD16x02"  
echo "**** **** **** **** **** **** **** ***" 
echo "---> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD16x2 --output-dir %homepath%\Downloads\hex\atmega\NANO_PRO_MINI_3_BUTTONS_LCD16x2
echo "---> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD20x4 --output-dir %homepath%\Downloads\hex\atmega\PRO_MINI_3_BUTTONS_LCD16x2_RTC


echo "\m\nArduino Atmega328 LCD20x04"   
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD16x2 --output-dir %homepath%\Downloads\hex\atmega\ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD20x4 --output-dir %homepath%\Downloads\hex\atmega\ARDUINO_3_BUTTONS_LCD20x4
echo "-------> ARDUINO_ENCODER_LCD20x4"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_ENCODER_LCD20x4 --output-dir %homepath%\Downloads\hex\atmega\ARDUINO_ENCODER_LCD20x4
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b arduino:avr:nano .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_ENCODER_RTC_LCD16x2 --output-dir %homepath%\Downloads\hex\atmega\ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> PRO_MINI_3_BUTTONS_LCD16x2_RTC"
arduino-cli compile -b arduino:avr:nano .\98_TESTS\PRO_MINI_3_BUTTONS_LCD16x2_RTC --output-dir %homepath%\Downloads\hex\atmega\PRO_MINI_3_BUTTONS_LCD16x2_RTC

echo "\n\nLGT8F328 LCD"
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b lgt8fx:avr:328 .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_ENCODER_RTC_LCD16x2 --output-dir %homepath%\Downloads\hex\lgt8fx\LGT8F_ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b lgt8fx:avr:328 .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD16x2 --output-dir %homepath%\Downloads\hex\lgt8fx\LGT8F_ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b lgt8fx:avr:328 .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD20x4 --output-dir %homepath%\Downloads\hex\lgt8fx\LGT8F_ARDUINO_3_BUTTONS_LCD20x4

echo "\n\nArduino Nano 33 IoT"
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile --fqbn arduino:samd:nano_33_iot .\10_NANO_33_IOT\NANO_IOT_SOCKET\QN8066_CONTROLLER --output-dir %homepath%\Downloads\hex\NANO33\QN8066_CONTROLLER
arduino-cli compile --fqbn arduino:samd:nano_33_iot .\10_NANO_33_IOT\NANO_IOT_ENCODER_LCD16x2 --output-dir %homepath%\Downloads\hex\NANO33\NANO_IOT_ENCODER_LCD16x2


echo "\n\nMiniCore - It can help you to save memory on ATmega328"
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_ENCODER_RTC_LCD16x2 --output-dir %homepath%\Downloads\hex\MiniCore\ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD16x2 --output-dir %homepath%\Downloads\hex\MiniCore\ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  .\03_TX_LCD_16x2_AND_20x4\01_ARDUINO\ARDUINO_3_BUTTONS_LCD20x4 --output-dir %homepath%\Downloads\hex\MiniCore\ARDUINO_3_BUTTONS_LCD20x4
echo "-------> STANDALONE_ATMEGA328_ENCODER_V1"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  .\98_TESTS\02_ATMEGA328\STANDALONE_ATMEGA328_ENCODER_V1 --output-dir %homepath%\Downloads\hex\MiniCore\03_LCD_16x2_AND_20x4_V1
echo "-------> STANDALONE_ATMEGA328_ENCODER_V2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  .\98_TESTS\02_ATMEGA328\STANDALONE_ATMEGA328_ENCODER_V2 --output-dir %homepath%\Downloads\hex\MiniCore\03_LCD_16x2_AND_20x4_V2

echo "\n\nESP32 -  To know your ESP32 device and detect the port: esptool.py flash_id "
echo "************************************"
echo "ESP32 Dev Module"
echo "ESP32 LCD16x2 AND ENCODER"
echo "**** **** **** **** **** **** **** ***"
echo "-------> 01_ESP32_LCD16x2_ENCODER"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso .\07_ESP32\01_ESP32_LCD16x2_ENCODER\ESP32_ENCODER --output-dir %homepath%\Downloads\hex\ESP32\DEVM\LCD16x2_encoder
echo "-------> 02_ESP32_WEB_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso .\07_ESP32\02_ESP32_WEB_WIFI_QN8066\ESP32_WEB_QN8066 --output-dir %homepath%\Downloads\hex\ESP32\DEVM\ESP32_ESP32_WEB_QN8066
echo "-------> 03_ESP32_PYTHON_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso .\07_ESP32\03_ESP32_PYTHON_WIFI_QN8066\SOCKETS_ESP32_QN8066 --output-dir %homepath%\Downloads\hex\ESP32\DEVM\ESP32_SOCKETS_ESP32_QN8066
echo "-------> 04_ESP32_WEB_ACTIVE_PORTAL_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso .\07_ESP32\04_ESP32_WEB_ACTIVE_PORTAL_QN8066\ESP32_ACTIVE_PORTAL_QN8066 --output-dir %homepath%\Downloads\hex\ESP32\DEVM\ESP32_ACTIVE_PORTAL_QN8066

echo "\n\nESP32C3 Dev Module"
echo "************************************"
echo "-------> 02_ESP32_WEB_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 .\07_ESP32\02_ESP32_WEB_WIFI_QN8066\ESP32_WEB_QN8066 --output-dir %homepath%\Downloads\hex\ESP32C3\DEVM\ESP32_ESP32_WEB_QN8066
echo "-------> 03_ESP32_PYTHON_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 .\07_ESP32\03_ESP32_PYTHON_WIFI_QN8066\SOCKETS_ESP32_QN8066 --output-dir %homepath%\Downloads\hex\ESP32C3\DEVM\ESP32_SOCKETS_ESP32_QN8066
echo "-------> 04_ESP32_WEB_ACTIVE_PORTAL_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 .\07_ESP32\04_ESP32_WEB_ACTIVE_PORTAL_QN8066\ESP32_ACTIVE_PORTAL_QN8066 --output-dir %homepath%\Downloads\hex\ESP32C3\DEVM\ESP32_ACTIVE_PORTAL_QN8066



echo "\n\nESP8266 Generic Module"
echo "************************************"
arduino-cli compile --fqbn esp8266:esp8266:generic .\09_ESP8266\ESP8266_ACCESS_POINT_WEB --output-dir %homepath%\Downloads\hex\ESP8266\DEVM\ESP8266_ACCESS_POINT_WEB



:: compiles STM32 sketch
echo "\n\nSTM32"
echo "************************************"
echo "-------> STM32_ENCODER_LCD"
:: arduino-cli board -b stm32duino:STM32F1:genericSTM32F103C  details
arduino-cli compile --fqbn STMicroelectronics:stm32:GenF1:pnum=BLUEPILL_F103C8 .\08_STM32\STM32_ENCODER_LCD --output-dir %homepath%\Downloads\hex\STM32\F1\STM32_ENCODER_LCD


echo "\n\nATTINY"
echo "************************************"
echo "-------> TX_ATTINY85_RDS_OLED"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx5:clock=1internal .\06_TX_ATTINY8X\TX_ATTINY85_RDS_OLED --output-dir %homepath%\Downloads\hex\ATTINY\85\TX_ATTINY85_RDS_OLED
echo "-------> TX_ATTINY44_RDS_OLED"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx4:chip=44,clock=1internal .\06_TX_ATTINY8X\TX_ATTINY44_RDS_OLED --output-dir %homepath%\Downloads\hex\ATTINY\44\TX_ATTINY44_RDS_OLED
echo "-------> TX_ATTINY24_RDS"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx4:chip=24,clock=1internal .\06_TX_ATTINY8X\TX_ATTINY24 --output-dir %homepath%\Downloads\hex\ATTINY\24\TX_ATTINY24


echo "\n\nRX - SAERIAL MONITOR"   
echo "************************************"
echo "-------> 50_RX_SERIAL_MONITOR"
arduino-cli compile -b arduino:avr:nano .\50_RX_SERIAL_MONITOR\01_RX_A --output-dir %homepath%\Downloads\hex\atmega\50_RX_A





