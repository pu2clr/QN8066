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
arduino-cli compile -b arduino:avr:nano ./01_SERIAL_MONITOR/A_MINIMALIST_TX --output-dir ~/Downloads/hex/atmega/A_MINIMALIST_TX
echo "---> A_TX"
arduino-cli compile -b arduino:avr:nano ./01_SERIAL_MONITOR/A_TX --output-dir ~/Downloads/hex/atmega/A_TX
echo "---> B_TX"
arduino-cli compile -b arduino:avr:nano ./01_SERIAL_MONITOR/B_TX --output-dir ~/Downloads/hex/atmega/B_TX
echo "---> TX_RDS"
arduino-cli compile -b arduino:avr:nano ./02_RDS/TX_RDS1 --output-dir ~/Downloads/hex/atmega/TX_RDS1
arduino-cli compile -b arduino:avr:nano ./02_RDS/TX_RDS2 --output-dir ~/Downloads/hex/atmega/TX_RDS2


echo "---> LCD16x02 - Minimalist"   
arduino-cli compile -b arduino:avr:nano ./03_LCD_16x2_AND_20x4/00_ARDUINO_MINIMALIST --output-dir ~/Downloads/hex/atmega/00_ARDUINO_MINIMALIST


echo "---> Arduino Atmega328 LCD16x02"   
arduino-cli compile -b arduino:avr:nano ./03_LCD_16x2_AND_20x4/00_ARDUINO_MINIMALIST --output-dir ~/Downloads/hex/atmega/LCD16x2_arduino


echo "---> Arduino Atmega328 LCD20x04"   
arduino-cli compile -b arduino:avr:nano ./03_LCD_16x2_AND_20x4/01_ARDUINO/NANO_PRO_MINI_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/atmega/LCD20x4_arduino


echo "MiniCore - It can help you to save memory on ATmega328"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_LCD_16x2_AND_20x4/02_ATMEGA328/STANDALONE_ATMEGA328_ENCODER --output-dir ~/Downloads/hex/MiniCore/03_LCD_16x2_AND_20x4

echo "ESP32"
echo "ESP32 LCD16x2 AND ENCODER"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./03_LCD_16x2_AND_20x4/03_ESP32/ESP32_ENCODER --output-dir ~/Downloads/hex/ESP32/DEVM/LCD16x2_encoder



echo "ATTINY85"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx5:clock=1internal ./06_ATTINY85_RDS_OLED --output-dir ~/Downloads/hex/ATTIMY85/ATTINY85

