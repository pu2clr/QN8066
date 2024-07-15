# QN8066 FM DSP RX/TX  Arduino Library

This library is intended to provide an easy-to-use interface for controlling the QN8066 FM transceiver. 

The primary goal of this Arduino library project for the QN8066 is to simplify the development of robust applications using this device. I hope that hobbyists, electronics technicians, radio amateurs, and others interested in this work will find success in their projects.


This library can be freely distributed using the [MIT Free Software model](https://github.com/pu2clr/QN8066/blob/main/LICENSE). 
By using or installing Library you are agreeing to the terms of the [MIT licence](https://github.com/pu2clr/QN8066/blob/main/LICENSE).

[Copyright (c) 2024 Ricardo Lima Caratti](https://github.com/pu2clr/QN8066/blob/main/LICENSE). 


## About Me

I hold a Master's degree in Educational Technology from the Federal University of Ceará, where I delved into the ways technology can enhance learning experiences. My passion for computer science led me to specialize in it, focusing on Distributed Systems Development with an Object-Oriented approach, at the University of Brasília. My academic journey began with a Bachelor's degree in Information Systems from the União Pioneira de Integração Social (UPIS-Brasília). Outside the classroom, my main hobbies are electronics and Amateur Radio. 

## Notice to Users Regarding Initial Version of the Library

This library is still in its initial version. Although the implemented functions have been tested, it is possible that in some situations it may present issues or not respond as expected. Therefore, I ask users of this library to contact me (pu2clr@gmail.com) to report any problems. I will do my best to resolve or clarify the issue as quickly as possible. Thank you.


## Contents

1. [Legal Compliance Guidelines](./#legal-compliance-guidelines)
2. [Technical Overview of the QN8066 RX/TX Device (IC)](./#technical-overview-of-the-qn8066-rxtx-device-ic)
3. [QN8066 Functional Blocks](./#qn8066-functional-blocks)
4. [QN8066 PINOUT](./#qn8066-pinout)
5. [QN8066 Summary of User Control Registers](./#qn8066-summary-of-user-control-registers)
6. [DIY Kit 5W-7W FM Transceiver](./#diy-kit-5w-7w-fm-transceiver)
7. [Arduino Library Documentation](https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html)
8. [Arduino Sketch examples](./examples/)
9. [Donate](./#donate)
10. [References](./#references)

## Legal Compliance Guidelines

### Important Notice

Please be aware that the use of FM transmitters is regulated by the laws and regulations of each country. It is crucial to ensure that your use of this transmitter complies with the applicable legal requirements and does not violate any broadcasting regulations.

### Legal Compliance Guidelines

1. **Frequency Range**: Ensure that the frequency range used by your FM transmitter is permitted in your country.
2. **Transmission Power**: Adhere to the maximum allowed transmission power to avoid interference with licensed broadcasters and other communication services.
3. **Licensing**: Check if a license is required to operate an FM transmitter in your region. Obtain the necessary permissions if required.
4. **Interference**: Avoid causing interference with other electronic devices and communication services. Ensure your setup follows the prescribed technical standards.
5. **Public Safety**: Be mindful of public safety and ensure your use of the FM transmitter does not pose any risks.

By following these guidelines, you can ensure that your use of the FM transmitter respects the laws and regulations of your country and does not result in any legal issues.


## Technical Overview of the QN8066 RX/TX Device (IC)

### Introduction

The QN8066 is a highly integrated and versatile receiver/transmitter (RX/TX) device designed for use in FM radio applications. It offers a comprehensive solution for high-quality audio reception and transmission with minimal external components, making it suitable for a wide range of applications, including portable radios, automotive systems, and multimedia devices.

#### Key Features

- **Frequency Range**: The QN8066 operates over a wide frequency range, supporting FM band frequencies from 64 MHz to 108 MHz.
- **High Sensitivity**: The device is engineered to provide excellent sensitivity, allowing for clear reception of weak signals.
- **Integrated Analog and Digital Functions**: It includes both analog and digital components, ensuring compatibility with various audio formats and enhancing overall functionality.
- **Stereo Decoder**: The integrated stereo decoder ensures high-fidelity stereo audio output.
- **Low Power Consumption**: Designed with power efficiency in mind, the QN8066 is suitable for battery-powered devices, extending operational life.
- **Digital Signal Processing (DSP)**: Advanced DSP capabilities improve signal quality and reduce noise and interference.
- **Programmable Interface**: The QN8066 offers a flexible, programmable interface, enabling easy integration and customization for specific applications.

#### Technical Specifications

- **Operating Voltage**: The device operates at a typical voltage range of 2.7V to 3.6V.
- **Current Consumption**: It features low current consumption, typically around 20mA during active operation.
- **Output Power**: The output power is adjustable, catering to different transmission requirements.
- **Modulation Modes**: Supports various modulation modes including mono, stereo, and digital audio formats.
- **Interface**: I2C interface for easy communication with microcontrollers and other digital systems.

#### Applications

- **Portable Radios**: Due to its compact size and low power consumption, the QN8066 is ideal for portable radio applications.
- **Automotive Systems**: The high sensitivity and DSP capabilities make it suitable for in-car entertainment systems.
- **Multimedia Devices**: Compatible with a variety of audio sources, the QN8066 can be integrated into multimedia devices requiring FM reception and transmission capabilities.


## QN8066 Functional Blocks


![Functional Blocks](./extras/images/BLOCK_DIA.png)


1. **RF Front-End**: The RF front-end is designed to efficiently capture and process FM signals, ensuring minimal loss and high-quality signal reception.
2. **PLL Synthesizer**: The phase-locked loop (PLL) synthesizer ensures stable frequency generation and accurate tuning.
3. **Audio Processing**: Includes both analog and digital audio processing units to enhance sound quality and support various audio formats.
4. **Power Management**: Advanced power management features optimize power consumption, making the QN8066 ideal for portable applications.



## QN8066 PINOUT 

![QN8066 PINOUT](./extras/images/QN8066_PINOUT.png)

### QN8066 Pin Descriptions

| SSOP16 PIN | NAME | DESCRIPTION |
| ------- | ---- | ----------- |
| 1       | SCL  | Clock for I2C serial bus |
| 2       | SDA  | Bi-directional data line for I2C serial bus |   
| 3       | VCC  | Voltage supply |
| 4       | ALO  | Analog audio output – left channel | 
| 5       | ARO  | Analog audio output – right channel |
| 6       | GND  | Ground | 
| 7       | RFI  | FM Receiver RF input |
| 8       | GND  | Ground |
| 9       | ARI  | Right channel audio input |
| 10      | ALI  | Left channel audio input | 
| 11      | GND  | Ground |
| 12      | RFO  | Transmitter RF output – connect to matched antenna |
| 13      | CEN  | Chip enable with internal pull up: Chip power down if less than 0 |4V and VIO is powered; power up if higher voltage or left floated |
| 14      | XCLK | External clock input | 
| 15      | GND Ground | 
| 16      | INT  | Interrupt output, active low, need pull-up externally | 



## QN8066 Summary of User Control Registers

| REGISTER  | NAME          | USER CONTROL FUNCTIONS |
| --------- | ------------- | ---------------------- | 
| 00h       | SYSTEM1       |  Sets device modes     |
| 01h       | SYSTEM2       |  Sets external clock type and CCA parameters |
| 02h       | CCA           | Sets CCA parameters |
| 03h       | SNR           | Estimate RF input CNR value |
| 04h       | RSSISIG       | In-band signal RSSI dBµ V value |
| 05h       | CID1          | Device ID numbers |
| 06h       | CID2          | Device ID numbers |
| 07h       | XTAL_DIV0     | Frequency select of reference clock source |
| 08h       | XTAL_DIV1     | Frequency select of reference clock source |
| 09h       | XTAL_DIV2     | Frequency select of reference clock source |
| 0Ah       | STATUS1       | System status |
| 0Bh       | RX_CH         | Lower 8 bit of 10-bit receiver channel index |
| 0Ch       | CH_START      | Lower 8 bits of 10-bit channel scan start channel index |
| 0Dh       | CH_STOP       | Lower 8 bits of 10-bit channel scan stop channel index |
| 0Eh       | CH_STEP       | Channel scan frequency step | Highest 2 bits of receiver channel indexes |
| 0Fh       | RX_RDSD0      | RDS data byte 0 |
| 10h       | RX_RDSD1      | RDS data byte 1 |
| 11h       | RX_RDSD2      | RDS data byte 2 |
| 12h       | RX_RDSD3      | RDS data byte 3 |
| 13h       | RX_RDSD4      | RDS data byte 4 |
| 14h       | RX_RDSD5      | RDS data byte 5 |
| 15h       | RX_RDSD6      | RDS data byte 6 |
| 16h       | RX_RDSD7      | RDS data byte 7 |
| 17h       | STATUS2       | Receiver RDS status indicators |
| 18h       | VOL_CTL       | Audio volume control |
| 19h       | INT_CTRL      | Receiver RDS control
| 1Ah       | STATUS3       | Receiver audio peak level and AGC status |
| 1Bh       | TXCH          | Lower 8 bit of 10-bit transmitter channel index |
| 1Ch       | TX_RDSD0      | Transmit RDS data byte0 |
| 1Dh       | TX_RDSD1      | Transmit RDS data byte1 |
| 1Eh       | TX_RDSD2      | Transmit RDS data byte2 |
| 1Fh       | TX_RDSD3      | Transmit RDS data byte3 |
| 20h       | TX_RDSD4      | Transmit RDS data byte4 | 
| 21h       | TX_RDSD5      | Transmit RDS data byte5 |
| 22h       | TX_RDSD6      | Transmit RDS data byte6 |
| 23h       | TX_RDSD7      | Transmit RDS data byte7 |
| 24h       | PAC           | PA output power target control | 
| 25h       | FDEV          | Specify total TX frequency deviation |
| 26h       | RDS           | Specify transmit RDS frequency deviation | 
| 27h       | GPLT          | Transmitter soft chip threshold, gain of TX pilot | 
| 28h       | REG_VGA       | TX AGC gain | 




## DIY Kit 5W-7W FM Transceiver

It is possible to find Transceivers based on the QN8066 in kit form. With this library, you will also be able to control this kit, thereby expanding its original functionalities. Check AliExpress and others to check it out. 

The images below show the DIY Kit 5W-7W FM Transceiver available on AliExpress.


### DIY Kit 5W-7W FM Transceiver assembled without the RF final amplifier 

![DIY Kit 5W-7W FM Transceiver 01](./extras/images/KIT_ALIEXPRESS_01.jpg)


### DIY Kit 5W-7W FM Transceiver assembled in a homemade case

![DIY Kit 5W-7W FM Transceiver 02](./extras/images/KIT_ALIEXPRESS_02.jpg)


### Using the QN8066 Arduino Library with DIY Kit 5W-7W FM Transceiver

The code below is a minimalist sketch example of using this library with the DIY Kit 5W-7W FM  board. 

#### Arduino and the  Kit 5W-7W FM connections

| Anduino Nano or Uno pin | Kit 5W-7W FM  |
| ----------------------- | ------------- | 
|          GND            |     GND       | 
|           D9            |     PWM       | 
|           A4            |     SDA       | 
|           A5            |     SCL       | 


```cpp
#include <QN8066.h>

QN8066 dv;


void setup() {

  Serial.begin(9600);
  while (!Serial); 

  pinMode(9, OUTPUT);  // Pin use to PWM power control
 
  dv.setup();
  Serial.print("\nStarting the system...");
  delay(1000);      
  dv.setTX(1067); // Set the transmitter to 106.7 MHz 
  dv.setTxStereo(true);
  analogWrite(9, 50);  // It is about 1/5 of the max power (5~7W). It is between 1 and 1,4 W
  Serial.print("\nBroadcasting...");
}

void loop() {
}
````

See also [DIY Kit 5W-7W FM example](https://github.com/pu2clr/QN8066/tree/main/examples/01_SERIAL_MONITOR/B_TX)


## Donate 

If you find this project useful, consider making a donation so that the author of this library can purchase components and modules for improvements and testing of this library. [Click here to donate](https://www.paypal.com/donate/?business=LLV4PHKTXC4JW&no_recurring=0&item_name=Consider+making+a+donation.+So%2C+I+can+purchase+components+and+modules+for+improvements+and+testing+of+this+library.&currency_code=USD) or use the QR code below.


![Donate](./extras/images/QR_Code.png) 


## References 

* [Data Sheet - QUINTIC QN8066 Digital FM Transceiver for Portable Devices](https://vrtp.ru/index.php?act=Attach&type=post&id=913773)
* [Dynamic_RDS - FM Transmitter Plugin for Falcon Player](https://github.com/ShadowLight8/Dynamic_RDS)

