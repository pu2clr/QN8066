# This script uses the arduino-cli to compile the arduino sketches using command line (without Arduino IDE).
# It is very useful to check the library on multiple board types  after bug fixes and improvements. 
# It runs on a MacOS but you can port it to Linux o Windows easily.
# Some compilation results (bin and hex files) will be stores in your Download directory (/Users/<username>/Downloads/hex)
# ATTENTION: be careful with --output-dir arduino-cli option. Before compiling, this option removes all the contents of the last level folder. 
#            For example: if you use "--output-dir ~/Downloads", all the current content of the Downloads folder will be lost. 
#                         if you use "--output-dir ~/Downloads/hex", all current content of the hex folder will be lost and the Downloads 
#                         content will be preserved. 
# Ricardo Lima Caratti Mar 2024

echo "Arduino ATmega328 based board"

arduino-cli compile -b arduino:avr:nano ./examples/01_SERIAL_MONITOR/A_MINIMALIST_TX --output-dir ~/Downloads/hex/atmega/A_MINIMALIST_TX
arduino-cli compile -b arduino:avr:nano ./examples/01_SERIAL_MONITOR/A_TX --output-dir ~/Downloads/hex/atmega/A_TX
arduino-cli compile -b arduino:avr:nano ./examples/01_SERIAL_MONITOR/B_TX --output-dir ~/Downloads/hex/atmega/B_TX
arduino-cli compile -b arduino:avr:nano ./examples/99_UNDER_CONSTRUCTION/C_TX_RDS --output-dir ~/Downloads/hex/atmega/C_TX_RDS
arduino-cli compile -b arduino:avr:nano ./examples/03_LCD_16x2_AND_20x4/NANO_QN8066_LCD16x02_V1/ --output-dir ~/Downloads/hex/atmega/LCD16xV1
arduino-cli compile -b arduino:avr:nano ./examples/03_LCD_16x2_AND_20x4/NANO_QN8066_LCD16x02_V2/ --output-dir ~/Downloads/hex/atmega/LCD16xV2

