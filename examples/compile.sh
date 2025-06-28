# This script uses the arduino-cli to compile the arduino sketches using command line (without Arduino IDE).
# It is very useful to check the library on multiple board types  after bug fixes and improvements. 
# It runs on a MacOS but you can port it to Linux o Windows easily.
# Some compilation results (bin and hex files) will be stores in your Download directory (/Users/<username>/Downloads/hex)
# ATTENTION: be careful with --output-dir arduino-cli option. Before compiling, this option removes all the contents of the last level folder. 
#            For example: if you use "--output-dir ~/Downloads", all the current content of the Downloads folder will be lost. 
#                         if you use "--output-dir ~/Downloads/hex", all current content of the hex folder will be lost and the Downloads 
#                         content will be preserved. 
#
# Ricardo Lima Caratti Mar 2024

# ARDUINO ATMEGA328 REQUIREMENTS:
# Arduino ATmega328 boards (Nano, Uno, Pro Mini) are included in the default Arduino IDE installation.
# To compile ATmega328 sketches, you need:
# arduino-cli core install arduino:avr

echo "\n\n**** Arduino ATmega328 based board ***"
echo "---> A_MINIMALIST_TX"
echo "**** **** **** **** **** **** **** ***"

arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/A_MINIMALIST_TX --output-dir ~/Downloads/hex/atmega/A_MINIMALIST_TX
echo "---> A_TX"
arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/A_TX --output-dir ~/Downloads/hex/atmega/A_TX
echo "---> B_TX"
arduino-cli compile -b arduino:avr:nano ./01_TX_SERIAL_MONITOR/B_TX --output-dir ~/Downloads/hex/atmega/B_TX
echo "---> TX_RDS1"
arduino-cli compile -b arduino:avr:nano ./02_TX_RDS/TX_RDS1 --output-dir ~/Downloads/hex/atmega/TX_RDS1
echo "---> TX_RDS2"
arduino-cli compile -b arduino:avr:nano ./02_TX_RDS/TX_RDS2 --output-dir ~/Downloads/hex/atmega/TX_RDS2

echo "\n\nLCD16x02 - Minimalist"   
echo "**** **** **** **** **** **** **** ***"
echo "---> 00_ARDUINO_MINIMALIST"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/00_ARDUINO_MINIMALIST --output-dir ~/Downloads/hex/atmega/00_ARDUINO_MINIMALIST


echo "\n\nArduino Atmega328 LCD16x02"  
echo "**** **** **** **** **** **** **** ***" 
echo "---> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/atmega/NANO_PRO_MINI_3_BUTTONS_LCD16x2
echo "---> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/atmega/PRO_MINI_3_BUTTONS_LCD16x2_RTC


echo "\m\nArduino Atmega328 LCD20x04"   
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/atmega/ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/atmega/ARDUINO_3_BUTTONS_LCD20x4
echo "-------> ARDUINO_ENCODER_LCD20x4"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_LCD20x4 --output-dir ~/Downloads/hex/atmega/ARDUINO_ENCODER_LCD20x4
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b arduino:avr:nano ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/atmega/ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> PRO_MINI_3_BUTTONS_LCD16x2_RTC"
arduino-cli compile -b arduino:avr:nano ./98_TESTS/PRO_MINI_3_BUTTONS_LCD16x2_RTC --output-dir ~/Downloads/hex/atmega/PRO_MINI_3_BUTTONS_LCD16x2_RTC

# LGT8F328 REQUIREMENTS:
# LGT8F328 is a cheaper alternative to ATmega328 with additional features.
# To compile LGT8F328 sketches, you need:
# arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json
# arduino-cli core update-index
# arduino-cli core install lgt8fx:avr

echo "\n\nLGT8F328 LCD"
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b lgt8fx:avr:328 ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/lgt8fx/LGT8F_ARDUINO_3_BUTTONS_LCD20x4

# ARDUINO NANO 33 IOT REQUIREMENTS:
# Arduino Nano 33 IoT uses SAMD21 microcontroller with WiFi capabilities.
# To compile Nano 33 IoT sketches, you need:
# arduino-cli core install arduino:samd

echo "\n\nArduino Nano 33 IoT"
echo "**** **** **** **** **** **** **** ***"
arduino-cli compile --fqbn arduino:samd:nano_33_iot ./10_NANO_33_IOT/NANO_IOT_SOCKET/QN8066_CONTROLLER --output-dir ~/Downloads/hex/NANO33/QN8066_CONTROLLER
arduino-cli compile --fqbn arduino:samd:nano_33_iot ./10_NANO_33_IOT/NANO_IOT_ENCODER_LCD16x2 --output-dir ~/Downloads/hex/NANO33/NANO_IOT_ENCODER_LCD16x2

# MINICORE REQUIREMENTS:
# MiniCore provides optimized bootloaders and configurations for ATmega328 to save memory.
# To compile with MiniCore, you need:
# arduino-cli config set board_manager.additional_urls https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json
# arduino-cli core update-index
# arduino-cli core install MiniCore:avr

echo "\n\nMiniCore - It can help you to save memory on ATmega328"
echo "**** **** **** **** **** **** **** ***"
echo "-------> ARDUINO_ENCODER_RTC_LCD16x2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_ENCODER_RTC_LCD16x2 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_ENCODER_RTC_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD16x2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD16x2 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_3_BUTTONS_LCD16x2
echo "-------> ARDUINO_3_BUTTONS_LCD20x4"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./03_TX_LCD_16x2_AND_20x4/01_ARDUINO/ARDUINO_3_BUTTONS_LCD20x4 --output-dir ~/Downloads/hex/MiniCore/ARDUINO_3_BUTTONS_LCD20x4
echo "-------> STANDALONE_ATMEGA328_ENCODER_V1"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./98_TESTS/02_ATMEGA328/STANDALONE_ATMEGA328_ENCODER_V1 --output-dir ~/Downloads/hex/MiniCore/03_LCD_16x2_AND_20x4_V1
echo "-------> STANDALONE_ATMEGA328_ENCODER_V2"
arduino-cli compile -b MiniCore:avr:328:bootloader=no_bootloader,LTO=Os_flto  ./98_TESTS/02_ATMEGA328/STANDALONE_ATMEGA328_ENCODER_V2 --output-dir ~/Downloads/hex/MiniCore/03_LCD_16x2_AND_20x4_V2

# ESP32 REQUIREMENTS:
# ESP32 is a powerful microcontroller with WiFi and Bluetooth capabilities.
# To compile ESP32 sketches, you need:
# arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
# arduino-cli core update-index
# arduino-cli core install esp32:esp32
# Note: To detect your ESP32 device and port, use: esptool.py flash_id

echo "\n\nESP32 -  To know your ESP32 device and detect the port: esptool.py flash_id "
echo "************************************"
echo "ESP32 Dev Module"
echo "ESP32 LCD16x2 AND ENCODER"
echo "**** **** **** **** **** **** **** ***"
echo "-------> 01_ESP32_LCD16x2_ENCODER"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./07_ESP32/01_ESP32_LCD16x2_ENCODER/ESP32_ENCODER --output-dir ~/Downloads/hex/ESP32/DEVM/LCD16x2_encoder
echo "-------> 02_ESP32_WEB_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./07_ESP32/02_ESP32_WEB_WIFI_QN8066/ESP32_WEB_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_ESP32_WEB_QN8066
echo "-------> 03_ESP32_PYTHON_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./07_ESP32/03_ESP32_PYTHON_WIFI_QN8066/SOCKETS_ESP32_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_SOCKETS_ESP32_QN8066
echo "-------> 04_ESP32_WEB_ACTIVE_PORTAL_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso ./07_ESP32/04_ESP32_WEB_ACTIVE_PORTAL_QN8066/ESP32_ACTIVE_PORTAL_QN8066 --output-dir ~/Downloads/hex/ESP32/DEVM/ESP32_ACTIVE_PORTAL_QN8066

# ESP32C3 REQUIREMENTS:
# ESP32-C3 is a RISC-V based microcontroller from Espressif with WiFi and Bluetooth LE.
# Uses the same ESP32 core package. Requirements are the same as ESP32:
# arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
# arduino-cli core update-index
# arduino-cli core install esp32:esp32

echo "\n\nESP32C3 Dev Module"
echo "************************************"
echo "-------> 02_ESP32_WEB_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 ./07_ESP32/02_ESP32_WEB_WIFI_QN8066/ESP32_WEB_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_ESP32_WEB_QN8066
echo "-------> 03_ESP32_PYTHON_WIFI_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 ./07_ESP32/03_ESP32_PYTHON_WIFI_QN8066/SOCKETS_ESP32_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_SOCKETS_ESP32_QN8066
echo "-------> 04_ESP32_WEB_ACTIVE_PORTAL_QN8066"
arduino-cli compile --fqbn esp32:esp32:esp32c3 ./07_ESP32/04_ESP32_WEB_ACTIVE_PORTAL_QN8066/ESP32_ACTIVE_PORTAL_QN8066 --output-dir ~/Downloads/hex/ESP32C3/DEVM/ESP32_ACTIVE_PORTAL_QN8066

# ESP8266 REQUIREMENTS:
# ESP8266 is a popular WiFi-enabled microcontroller with limited resources but great for IoT projects.
# To compile ESP8266 sketches, you need:
# arduino-cli config set board_manager.additional_urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
# arduino-cli core update-index
# arduino-cli core install esp8266:esp8266

echo "\n\nESP8266 Generic Module"
echo "************************************"
arduino-cli compile --fqbn esp8266:esp8266:generic ./09_ESP8266/ESP8266_ACCESS_POINT_WEB --output-dir ~/Downloads/hex/ESP8266/DEVM/ESP8266_ACCESS_POINT_WEB

# STM32 REQUIREMENTS:
# STM32 microcontrollers offer high performance with ARM Cortex cores.
# To compile STM32 sketches, you need:
# arduino-cli config set board_manager.additional_urls https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
# arduino-cli core update-index
# arduino-cli core install STMicroelectronics:stm32
# Note: Some STM32 boards may require additional drivers or specific upload methods

# compiles STM32 sketch
echo "\n\nSTM32"
echo "************************************"
echo "-------> STM32_ENCODER_LCD"
# arduino-cli board -b stm32duino:STM32F1:genericSTM32F103C  details
arduino-cli compile --fqbn STMicroelectronics:stm32:GenF1:pnum=BLUEPILL_F103C8 ./08_STM32/STM32_ENCODER_LCD --output-dir ~/Downloads/hex/STM32/F1/STM32_ENCODER_LCD

# ATTINY REQUIREMENTS:
# ATtiny microcontrollers are small, low-power chips perfect for minimal projects.
# To compile ATtiny sketches, you need:
# arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json
# arduino-cli core update-index
# arduino-cli core install ATTinyCore:avr
# Note: ATtiny chips have limited memory and I/O, choose libraries carefully

echo "\n\nATTINY"
echo "************************************"
echo "-------> TX_ATTINY85_RDS_OLED"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx5:clock=1internal ./06_TX_ATTINY8X/TX_ATTINY85_RDS_OLED --output-dir ~/Downloads/hex/ATTINY/85/TX_ATTINY85_RDS_OLED
echo "-------> TX_ATTINY44_RDS_OLED"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx4:chip=44,clock=1internal ./06_TX_ATTINY8X/TX_ATTINY44_RDS_OLED --output-dir ~/Downloads/hex/ATTINY/44/TX_ATTINY44_RDS_OLED
echo "-------> TX_ATTINY24_RDS"
arduino-cli compile --fqbn ATTinyCore:avr:attinyx4:chip=24,clock=1internal ./06_TX_ATTINY8X/TX_ATTINY24 --output-dir ~/Downloads/hex/ATTINY/24/TX_ATTINY24

# RASPBERRY PI PICO REQUIREMENTS:
# To compile Raspberry Pi Pico sketches, you need to install the RP2040 board support:
# arduino-cli core install rp2040:rp2040
# You also need to install the required libraries:
# arduino-cli lib install "LiquidCrystal I2C" (for LCD examples)

echo "\n\nRaspberry Pi Pico"
echo "************************************"
echo "-------> BASIC_TX_PICO"
arduino-cli compile --fqbn rp2040:rp2040:rpipico ./11_RASPBERRY_PI_PICO/BASIC_TX_PICO --output-dir ~/Downloads/hex/PICO/BASIC_TX_PICO
echo "-------> PICO_LCD16x2_ENCODER"
arduino-cli compile --fqbn rp2040:rp2040:rpipico ./11_RASPBERRY_PI_PICO/PICO_LCD16x2_ENCODER --output-dir ~/Downloads/hex/PICO/PICO_LCD16x2_ENCODER


echo "\n\nRX - SAERIAL MONITOR"   
echo "************************************"
echo "-------> 50_RX_SERIAL_MONITOR"
arduino-cli compile -b arduino:avr:nano ./50_RX_SERIAL_MONITOR/01_RX_A --output-dir ~/Downloads/hex/atmega/50_RX_A





