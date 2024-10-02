# This script uses the arduino-cli to compile the arduino sketches using command line (without Arduino IDE).
# It is very useful to check the library on multiple board types  after bug fixes and improvements. 
# It runs on a MacOS but you can port it to Linux o Windows easily.
# Some compilation results (bin and hex files) will be stores in your Download directory (/Users/<username>/Downloads/hex)
# ATTENTION: be careful with --output-dir arduino-cli option. Before compiling, this option removes all the contents of the last level folder. 
#            For example: if you use "--output-dir ~/Downloads", all the current content of the Downloads folder will be lost. 
#                         if you use "--output-dir ~/Downloads/hex", all current content of the hex folder will be lost and the Downloads 
#                         content will be preserved. 
# Ricardo Lima Caratti Mar 2024

echo "**** Arduino ATmega328 based board ***"
echo "---> A_MINIMALIST_TX"
echo "**** **** **** **** **** **** **** ***"

arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/A_MINIMALIST_TX --output-dir ~/Downloads/hex/atmega/A_MINIMALIST_TX
echo "---> A_TX"
arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/A_TX --output-dir ~/Downloads/hex/atmega/A_TX
echo "---> B_TX"
arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/B_TX --output-dir ~/Downloads/hex/atmega/B_TX
echo "---> TX_RDS"
arduino-cli compile -b arduino:avr:nano ./02_TX_RDS/TX_RDS1 --output-dir ~/Downloads/hex/atmega/TX_RDS1
arduino-cli compile -b arduino:avr:nano ./02_TX_RDS/TX_RDS2 --output-dir ~/Downloads/hex/atmega/TX_RDS2

echo " "
echo "---> LCD16x02 - Minimalist"   
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/00_ARDUINO_MINIMALIST --output-dir ~/Downloads/hex/atmega/00_ARDUINO_MINIMALIST


echo "---> Arduino Atmega328 LCD16x02"  
echo "**** **** **** **** **** **** **** ***" 
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/atmega/NANO_PRO_MINI_3_BUTTONS_LCD16x2
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/atmega/PRO_MINI_3_BUTTONS_LCD16x2_RTC


echo "---> Arduino Atmega328 LCD20x04"   
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/atmega/ARDUINO_3_BUTTONS_LCD16x2
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/atmega/ARDUINO_3_BUTTONS_LCD20x4
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_LCD20x4 --output-dir ~/Downloads/hex/atmega/ARDUINO_ENCODER_LCD20x4
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/atmega/ARDUINO_ENCODER_RTC_LCD16x2
arduino-cli compile -b arduino:avr:nano ./98_TESTS/PRO_MINI_3_BUTTONS_LCD16x2_RTC --output-dir ~/Downloads/hex/atmega/PRO_MINI_3_BUTTONS_LCD16x2_RTC

echo "---> LGT8F328 LCD"
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_ENCODER_RTC_LCD16x2
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_3_BUTTONS_LCD16x2
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_3_BUTTONS_LCD20x4


echo "MiniCore - It can help you to save memory on ATmega328"
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_ENCODER_RTC_LCD16x2
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_3_BUTTONS_LCD16x2
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_3_BUTTONS_LCD20x4
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./98_TESTS/02_ATMEGA328/STANDALONE_ATMEGA328_ENCODER_V1 --output-dir ~/Downloads/hex/MiniCore/03_LCD_16x2_AND_20x4_V1
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./98_TESTS/02_ATMEGA328/STANDALONE_ATMEGA328_ENCODER_V2 --output-dir ~/Downloads/hex/MiniCore/03_LCD_16x2_AND_20x4_V2


echo "ESP32 -  To know your ESP32 device and detect the port: esptool.py flash_id 
echo "************************************"
echo "ESP32 Dev Module"
echo "ESP32 LCD16x2 AND ENCODER"
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./03_TX_LCD_16x2_AND_20x4/03_ESP32/ESP32_ENCODER --output-dir ~/Downloads/hex/ESP32/DEVM/LCD16x2_encoder
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./07_WEB_WIFI_ESP32_QN8066/ESP32_WEB_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_ESP32_WEB_QN8066
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./08_PYTHON_WIFI_ESP32_QN8066/SOCKETS_ESP32_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_SOCKETS_ESP32_QN8066
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./09_WEB_ACTIVE_PORTAL_ESP32_QN8066/ESP32_ACTIVE_PORTAL_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_ACTIVE_PORTAL_QN8066

echo "ESP32C3 Dev Module"

arduino-cli compile --fqbn esp32:esp32:esp32c3 ./07_WEB_WIFI_ESP32_QN8066/ESP32_WEB_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_ESP32_WEB_QN8066
arduino-cli compile --fqbn esp32:esp32:esp32c3 ./08_PYTHON_WIFI_ESP32_QN8066/SOCKETS_ESP32_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_SOCKETS_ESP32_QN8066
arduino-cli compile --fqbn esp32:esp32:esp32c3 ./09_WEB_ACTIVE_PORTAL_ESP32_QN8066/ESP32_ACTIVE_PORTAL_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_ACTIVE_PORTAL_QN8066


echo "ATTINY85"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx5:clock=1internal ./06_TX_ATTINY8X/06_TX_ATTINY85_RDS_OLED --output-dir ~/Downloads/hex/ATTIMY85/06_TX_ATTINY85_RDS_OLED

echo "---> RX - SAERIAL MONITOR"   
arduino-cli compile -b arduino:avr:nano ./50_RX_SERIAL_MONITOR/01_RX_A --output-dir ~/Downloads/hex/atmega/50_RX_A



