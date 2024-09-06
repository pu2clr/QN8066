# EXTRA FILES AND DOCUMENTATION

In this folder, you will find extra files such as images, documentation of the library functions, and tips on configuring the Arduino environment.


## Arduino CLI - A faster alternative to the Arduino IDE

Arduino CLI (arduino-cli) is a solution that allows you to compile, build, upload, manage boards and libraries via shell command line. This way, you do not need to use the traditional Arduino IDE. Depending on the development environment you use, arduino-cli may be a good choice given it is conservative on system resources. You will notice that the arduino-cli compiles and uploades code faster. However, it is a command line program, and may present a steeper learning curve over teh Arduio IDE. The links below can help you to learn more about arduino-cli.

* [Click here for more detail about arduino-cli](https://arduino.github.io/arduino-cli/0.21/).
* [Getting started](https://arduino.github.io/arduino-cli/0.21/getting-started/)
* [Click here to watch a video about the arduino-cli](https://youtu.be/J-qGn1eEidA)


### Fast way to install arduino-cli and setup your enviroment with PU2CLR SI4735 Arduino Library and accessories

#### On macOS and Linux

Open the terminal and run the command below.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/SI4735/master/examples/lib_si4735_basic_install.sh | sh
```

The command above will install arduino-cli, the SI4735 Arduino Library, the basic libraries for OLED, LCD and TFT used in some examples, and the basic boards for Atmega328 and LGT8F328.

If you want to install all boards (esp32, ESP8266, STM32 etc) and all libraries used by the SI473X examples, run the command below after running the previous command.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/SI4735/master/examples/install_all_libraries_and_boards.sh | sh
```


#### On Windows 10 and 11

Open the cmd (terminal) and run

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/SI4735/master/examples/lib_si4735_basic_install.bat --output lib_si4735_basic_install.bat
.\lib_si4735_basic_install.bat
```
The commands above will install arduino-cli.exe in your home direcory, the SI4735 Arduino Library and the basic libraries for OLED, LCD and TFT used in some examples (in you Documents/Arduino folder), and the basic boards for Atmega328 and LGT8F328 (in AppData\local\Arduino15 folder).

After running the previous steps, if you want to configure the SI4735 Arduino Library for all libraries and boards used by the examples, run the command below.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/SI4735/master/examples/install_all_libraries_and_boards.bat --output install_all_libraries_and_boards.bat
.\install_all_libraries_and_boards.bat
```

### Othe ways to install the arduino-cli (without PU2CLR SI4735 Arduino Library setup)


### [Installing on Linux, MacOS or Windows)](https://arduino.github.io/arduino-cli/0.30/installation/)


#### On macOS

```bash
$ brew update
$ brew install arduino-cli
```

#### On Linux or macOS

```bash
$ curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```


#### On Windows

* Download the install file: https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.msi
* Download the exe (binary)file: https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip

### arduino-cli and main command examples

All boards and libraries installed in either the Arduino IDE and arduino-cli are available in the other. If you install a library or board in the arduino-cli, the Arduino IDE will have access to it as well, and vice-versa.

### The command below identifies the boards connected to your computer (COM3, COM4, /dev/xxx).

It is very useful to show the boards connected to your computer

```bash
$ arduino-cli board list

Port                            Protocol Type              Board Name                     FQBN                          Core       
/dev/cu.usbmodem14201           serial   Serial Port (USB) Arduino Due (Programming Port) arduino:sam:arduino_due_x_dbg arduino:sam
/dev/cu.usbserial-1410          serial   Serial Port (USB) Unknown

```

### The command below shows all boards available

It is very useful to give you the FQBN information to compile and upload the right board.

```bash
$ arduino-cli board listall
```

### The command below shows the board installed in your environment.

```bash
$ arduino-cli core list

Board Name                                       FQBN
3D printer boards                                STM32:stm32:3dprinter
4D Systems gen4 IoD Range                        esp8266:esp8266:gen4iod
AI Thinker ESP32-CAM                             esp32:esp32:esp32cam
.
.
.
Arduino Due (Native USB Port)                    arduino:sam:arduino_due_x
Arduino Due (Programming Port)                   arduino:sam:arduino_due_x_dbg
.
.
.
Arduino Nano                                     arduino:avr:nano
Arduino Nano 33 BLE                              arduino:mbed_nano:nano33ble
.
.
.
Arduino Uno                                      arduino:avr:uno
```

#### Example:

```bash
$ arduino-cli core list


ID                   Installed Latest Name
arduino:avr          1.8.5     1.8.5  Arduino AVR Boards
arduino:mbed_nano    3.0.0     3.0.0  Arduino Mbed OS Nano Boards
arduino:mbed_rp2040  3.0.0     3.0.0  Arduino Mbed OS RP2040 Boards
arduino:sam          1.6.12    1.6.12 Arduino SAM Boards (32-bits ARM Cortex-M3)
arduino:samd         1.8.13    1.8.13 Arduino SAMD Boards (32-bits ARM Cortex-M0+)
atmel-avr-xminis:avr 0.6.0     0.6.0  Atmel AVR Xplained-minis
attiny:avr           1.0.2     1.0.2  ATtiny Microcontrollers
ATTinyCore:avr       1.5.2     1.5.2  ATTinyCore
esp32:esp32          2.0.2     2.0.2  ESP32 Arduino
esp8266:esp8266      3.0.2     3.0.2  ESP8266 Boards (3.0.2)
MegaCore:avr         2.1.3     2.1.3  MegaCore
MightyCore:avr       2.1.3     2.1.3  MightyCore
MiniCore:avr         2.1.3     2.1.3  MiniCore
rp2040:rp2040        1.13.0    1.13.0 Raspberry Pi RP2040 Boards(1.13.0)
Seeeduino:samd       1.8.2     1.8.2  Seeed SAMD (32-bits ARM Cortex-M0+ and Cortex-M4) Boards
STM32:stm32          1.9.0     1.9.0  STM32 Boards (selected from submenu)
teensy:avr           1.56.1    1.56.1 Teensyduino
```


### The command below just compiles the sketch SI47XX_01_SERIAL_MONITOR/SI4735_01_POC for an Arduino Nano

```bash
$ arduino-cli compile -b arduino:avr:nano ./SI47XX_01_SERIAL_MONITOR/SI4735_01_POC
```

### The command below compiles and uploads the sketch into the Arduino Nano board

```bash
$ arduino-cli compile  -b arduino:avr:nano -u -p yourPort ./SI47XX_01_SERIAL_MONITOR/SI4735_01_POC.
```
Where __yourPort__ can be COM3, COM4, COM(N) if you are using Microsoft Windows; or /dev/XXX if you are using unix like SO (Linux or  MacOS).


### Example using Arduino DUE connected to a MACOS

```bash
$ arduino-cli compile --fqbn arduino:sam:arduino_due_x -u -p /dev/cu.usbmodem14201  ./SI47XX_10_RDS/SI47XX_02_RDS_TFT_TOUCH_SHIELD_35_V2
```


__See command arduino-cli board list below to know how to get the port.__


### Example using Arduino Mega

```bash
$ arduino-cli compile --fqbn arduino:avr:mega -u -p /dev/cu.usbserial-1410  ./SI47XX_10_RDS/SI47XX_02_RDS_TFT_TOUCH_SHIELD_35_V2
```

If you need the .hex, .elf or  .bin files you can use

```bash
$ arduino-cli compile --fqbn arduino:avr:mega --build-path /Users/UserName/Downloads ./SI47XX_10_RDS/SI47XX_02_RDS_TFT_TOUCH_SHIELD_35_V2
```

### Example using ESP32

#### Example using ESP32 DEVMODE

```bash
$ arduino-cli compile --fqbn esp32:esp32:esp32-poe-iso -u -dev -u -p /dev/cu.usbserial-0001  ./SI47XX_KITS/THIAGO_LIMA/GERT_BAAK/SI4735_2.8_TFT_SI5351_V4.2
```
where /dev/cu.usbserial-0001 is the Port device. It can be different in your environment. Check it via __arduino-cli board list__ command.

#### Example using ESP32 LOLIN32

```bash
arduino-cli compile --fqbn esp32:esp32:lolin32 -u -p /dev/cu.usbserial-00874A3C ./SI47XX_KITS/THIAGO_LIMA/GERT_BAAK/SI4735_2.8_TFT_SI5351_V4.2 -v
```

### Example using STM32 (compiling only)

arduino-cli compile --fqbn STM32:stm32:Nucleo_64 ./SI47XX_KITS/PLAMEN/PU2CLR_SI5351_SI4732_STM32


#### To know the right port you have to use, try the command below:

```bash
$ arduino-cli board list
```


#### arduino-cli board list command example

```bash
arduino-cli board list
Port                            Protocol Type              Board Name                     FQBN                          Core       
/dev/cu.usbmodem14201           serial   Serial Port (USB) Arduino Due (Programming Port) arduino:sam:arduino_due_x_dbg arduino:sam
```

#### arduino-cli compile and upload on Arduino DUE

```bash
$ arduino-cli compile --fqbn arduino:sam:arduino_due_x -u -p /dev/cu.usbmodem14201  ./SI47XX_10_RDS/SI47XX_02_RDS_TFT_TOUCH_SHIELD_35_V2

Sketch uses 90896 bytes (17%) of program storage space. Maximum is 524288 bytes.
Atmel SMART device 0x285e0a60 found
Erase flash
done in 0.033 seconds

Write 93876 bytes to flash (367 pages)
[==============================] 100% (367/367 pages)
done in 18.038 seconds
Set boot flash true
CPU reset.
```

##### arduino-cli board list command with two devices connected to the computer at the same time (Arduino DUE and Arduino Mega)

The command below show two Arduino boards connected to the computer at the same time. The first is an Arduino DUE as deteiled below and the second is an Arduino Mega with no details.
Unfortunatly, in some cases, the Arduino IDE and arduino-cli can not show details about the divice connected to the computer. In this case, you have to conclude this by yourself.

```bash
$ arduino-cli board list

Port                            Protocol Type              Board Name                     FQBN                          Core       
/dev/cu.usbmodem14201           serial   Serial Port (USB) Arduino Due (Programming Port) arduino:sam:arduino_due_x_dbg arduino:sam
/dev/cu.usbserial-1410          serial   Serial Port (USB) Unknown

```

##### arduino-cli board list command with two devices connected at the same time (Arduino Yún and Arduino Micro)

```bash

$ arduino-cli board list

Port                            Protocol Type              Board Name    FQBN              Core
/dev/cu.usbmodem14101           serial   Serial Port (USB) Arduino Yún   arduino:avr:yun   arduino:avr
/dev/cu.usbmodem14201           serial   Serial Port (USB) Arduino Micro arduino:avr:micro arduino:avr
```


The table below shows some Arduino board FQBN

| Board Name | FQBN |
| ---------- | ---- |
| Arduino DUE                         | arduino:sam:arduino_due_x |
| Arduino Duemilanove or Diecimila    | arduino:avr:diecimila     |
| Arduino Ethernet                    | arduino:avr:ethernet      |                                                                
| Arduino Industrial 101              | arduino:avr:chiwawa       |
| Arduino Leonardo                    | arduino:avr:leonardo      |
| Arduino M0                          | arduino:samd:mzero_bl     |
| Arduino M0 Pro (Native USB Port)    | arduino:samd:mzero_pro_bl |
| Arduino M0 Pro (Programming Port)   | arduino:samd:mzero_pro_bl_dbg |
| Arduino MKR WiFi 1010               | arduino:samd:mkrwifi1010  |
| Arduino Mega 2560                   | arduino:avr:mega          |
| Arduino Micro                       | arduino:avr:micro         |
| Arduino Mini                        | arduino:avr:mini          |
| Arduino NANO 33 IoT                 | arduino:samd:nano_33_iot  |
| Arduino Nano                        | arduino:avr:nano          |
| Arduino Pro or Pro Mini             | arduino:avr:pro           |
| Arduino Uno                         | arduino:avr:uno           |
| Arduino Uno WiFi                    | arduino:avr:unowifi       |
| Arduino Yún                         | arduino:avr:yun           |
| Arduino Yún Mini                    | arduino:avr:yunmini       |
| Arduino Zero (Native USB Port)      | arduino:samd:arduino_zero_native |
| Arduino Zero (Programming Port)     | arduino:samd:arduino_zero_edbg  |
| ESP32 Dev Mode                      | esp32:esp32:esp32-poe-is  |
| ESP32 LOLIN                         | esp32:esp32:lolin32       |
| ESP8266                             | esp8266:esp8266:generic   |
| Raspberry Pi Pico                   | rp2040:rp2040:rpipico     |
| Seeeduino XIAO                      | Seeeduino:samd:seeed_XIAO_m0 |
| STM32                               | STM32:stm32:Nucleo_64 |

type __arduino-cli board listall__ to see all boards available.


### arcuino-cli references

* [Getting Started with arduino-cli](https://create.arduino.cc/projecthub/B45i/getting-started-with-arduino-cli-7652a5)
* [Arduino CLI and the art of command line](https://youtu.be/cVod8k713_8)
* [Getting started](https://arduino.github.io/arduino-cli/0.21/getting-started/)
* [Arduino CLI: Getting Started](https://youtu.be/J-qGn1eEidA)
* [Arduino-cli - github.com](https://github.com/arduino/arduino-cli)
* [Arduino-cli: compile, upload and manage libraries, cores, and boards](https://www.pcbway.com/blog/Activities/Arduino_cli__compile__upload_and_manage_libraries__cores__and_boards.html)

