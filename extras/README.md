# EXTRA FILES AND DOCUMENTATION

In this folder, you will find extra files such as images, documentation of the library functions, and tips on configuring the Arduino environment.


## Arduino CLI - A faster alternative to the Arduino IDE

Arduino CLI (arduino-cli) is a solution that allows you to compile, build, upload, manage boards and libraries via shell command line. This way, you do not need to use the traditional Arduino IDE. Depending on the development environment you use, arduino-cli may be a good choice given it is conservative on system resources. You will notice that the arduino-cli compiles and uploades code faster. However, it is a command line program, and may present a steeper learning curve over teh Arduio IDE. The links below can help you to learn more about arduino-cli.

* [Click here for more detail about arduino-cli](https://arduino.github.io/arduino-cli/0.21/).
* [Getting started](https://arduino.github.io/arduino-cli/0.21/getting-started/)
* [Click here to watch a video about the arduino-cli](https://youtu.be/J-qGn1eEidA)


### Fast way to install arduino-cli and setup your enviroment with PU2CLR QN8066 Arduino Library and accessories

#### On macOS and Linux

Open the terminal and run the command below.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/QN8066/master/examples/setup_libraries_boards.sh | sh
```

The command above will install arduino-cli, the QN8066 Arduino Library, the basic libraries for OLED, LCD and TFT used in some examples, and the basic boards for Atmega328 and LGT8F328.

If you want to install all boards (esp32, ESP8266, STM32 etc) and all libraries used by the SI473X examples, run the command below after running the previous command.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/QN8066/master/examples/setup_libraries_boards.sh| sh
```


#### On Windows 10 and 11

Open the cmd (terminal) and run

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/QN8066/master/examples/setup_libraries_boards.bat --output lib_QN8066_basic_install.bat
.\lib_QN8066_basic_install.bat
```
The commands above will install arduino-cli.exe in your home direcory, the QN8066 Arduino Library and the basic libraries for OLED, LCD and TFT used in some examples (in you Documents/Arduino folder), and the basic boards for Atmega328 and LGT8F328 (in AppData\local\Arduino15 folder).

After running the previous steps, if you want to configure the QN8066 Arduino Library for all libraries and boards used by the examples, run the command below.

```bash
curl -fsSL https://raw.githubusercontent.com/pu2clr/QN8066/master/examples/setup_libraries_boards.bat --output install_all_libraries_and_boards.bat
.\install_all_libraries_and_boards.bat
```

### Othe ways to install the arduino-cli (without PU2CLR QN8066 Arduino Library setup)


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

See examples of compiling sketch via arduino-cli: https://github.com/pu2clr/QN8066/blob/main/examples/compile.sh



## Donate 

If you find this project useful, consider making a donation so that the author of this library can purchase components and modules for improvements and testing of this library. [Click here to donate](https://www.paypal.com/donate/?business=LLV4PHKTXC4JW&no_recurring=0&item_name=Consider+making+a+donation.+So%2C+I+can+purchase+components+and+modules+for+improvements+and+testing+of+this+library.&currency_code=USD) or use the QR code below.

