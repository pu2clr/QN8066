@ECHO OFF
REM This script installs all necessary Arduino board packages and libraries required for QN8066 library examples.
REM It is useful for setting up a development environment on Windows CMD/PowerShell.
REM
REM Prerequisites:
REM - arduino-cli must be installed and accessible from command line
REM - Internet connection is required for downloading packages
REM
REM Usage: Run this script once before using compile.bat
REM
REM Ricardo Lima Caratti 2024

echo.
echo ==========================================
echo Arduino CLI Configuration for QN8066 Library
echo ==========================================
echo.

REM Check if arduino-cli is installed, if not, install it
where arduino-cli >nul 2>nul
if %errorlevel% neq 0 (
    echo Arduino CLI not found. Installing arduino-cli...
    cd %USERPROFILE%
    echo Downloading arduino-cli.exe to your home folder...
    curl -fsSL https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip --output arduinocli.zip
    tar -xf arduinocli.zip 
    del arduinocli.zip
    echo arduino-cli installed to %USERPROFILE%
    echo Please add %USERPROFILE% to your PATH environment variable if not already added
    echo.
)

arduino-cli config set network.connection_timeout 600s

echo Initializing arduino-cli configuration...
arduino-cli config init

echo.
echo Updating arduino-cli core index...
arduino-cli core update-index

echo.
echo Configuring additional board manager URLs...

echo Adding LGT8F328 board support...
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json --append

echo Adding MiniCore support...
arduino-cli config set board_manager.additional_urls https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json --append

echo Adding ESP32 board support...
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json --append

echo Adding ESP8266 board support...
arduino-cli config set board_manager.additional_urls http://arduino.esp8266.com/stable/package_esp8266com_index.json --append

echo Adding STM32 board support...
arduino-cli config set board_manager.additional_urls https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json --append

echo Adding ATtiny board support...
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json --append

echo Adding Raspberry Pi Pico support...
arduino-cli config set board_manager.additional_urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json --append

echo.
echo Updating board manager index with new URLs...
arduino-cli core update-index

echo.
echo Installing board packages (this may take several minutes)...

echo Installing default Arduino AVR boards (Uno, Nano, Pro Mini)...
arduino-cli core install arduino:avr

echo Installing Arduino SAMD boards (Nano 33 IoT)...
arduino-cli core install arduino:samd

echo Installing LGT8F328 board package...
arduino-cli core install lgt8fx:avr

echo Installing MiniCore package...
arduino-cli core install MiniCore:avr

echo Installing ESP32 board package...
arduino-cli core install esp32:esp32

echo Installing ESP8266 board package...
arduino-cli core install esp8266:esp8266

echo Installing STM32 board package...
arduino-cli core install STMicroelectronics:stm32

echo Installing ATtiny board package...
arduino-cli core install ATTinyCore:avr

echo Installing Raspberry Pi Pico (RP2040) board support...
arduino-cli core install rp2040:rp2040

echo.
echo Installing required libraries...

echo Installing QN8066 library...
arduino-cli lib install "QN8066"

echo Installing LiquidCrystal I2C library (for LCD examples)...
arduino-cli lib install "LiquidCrystal I2C"

echo Installing LiquidCrystal library...
arduino-cli lib install "LiquidCrystal"

echo Installing DS1302 RTC library...
arduino-cli lib install "Ds1302"

echo Installing WiFiNINA library (for Arduino Nano 33 IoT)...
arduino-cli lib install "WiFiNINA"

echo Installing FlashStorage library...
arduino-cli lib install "FlashStorage"

echo Installing STM32duino RTC library...
arduino-cli lib install "STM32duino RTC"

echo Installing ArduinoJson library...
arduino-cli lib install "ArduinoJson"

echo Installing Tiny4kOLED library (for OLED displays)...
arduino-cli lib install "Tiny4kOLED"

echo Installing TinyOLED-Fonts library...
arduino-cli lib install "TinyOLED-Fonts"

echo.
echo Installing additional libraries via zip file...
echo Installing Adafruit_SH1106 library...
SET ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL=true
curl -fsSL https://github.com/wonho-maker/Adafruit_SH1106/archive/refs/heads/master.zip --output SH1106.zip
arduino-cli lib install --zip-path ./SH1106.zip
del SH1106.zip

echo.
echo ==========================================
echo Setup Complete!
echo ==========================================
echo.
echo All board packages and libraries have been installed.
echo You can now run compile.bat to compile all examples.
echo.
echo Note: The Nokia 5110 LCD library (LCD5110_Graph) needs to be installed manually
echo as the site does not allow deep linking. If you need Nokia 5110 examples:
echo 1. Download from: http://www.rinkydinkelectronics.com/library.php?id=47
echo 2. Run: arduino-cli lib install --zip-path path\to\LCD5110_Graph.zip
echo.
echo If you encounter any issues, please ensure that:
echo - arduino-cli is properly installed and in your PATH
echo - You have an active internet connection
echo - You have sufficient disk space for all packages
echo.
pause
