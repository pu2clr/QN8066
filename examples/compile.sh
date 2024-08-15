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

echo "---> NANO_QN8066_LCD16x02_V3"   
arduino-cli compile -b arduino:avr:nano ./03_LCD_16x2_AND_20x4/01_ARDUINO/NANO_PRO_MINI_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/atmega/LCD16x2_arduino

echo "ATTINY85"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx5:clock=1internal ./06_ATTINY85_RDS_OLED --output-dir ~/Downloads/hex/ATTIMY85/ATTINY85

