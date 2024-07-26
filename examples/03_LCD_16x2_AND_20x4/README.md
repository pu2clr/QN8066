# QN8066 Arduino Library and LCD 16x02 examples


## Arduino Nano and "KIT DIY 5~7W FM TRANSMITTER" schematic

The following schematic illustrates the connections between the Arduino Nano, the "KIT DIY 5~7W FM TRANSMITTER," and either the LCD 16x2 or the LCD 20x4. It is important to note that both the LCD 16x2 and the LCD 20x4 have the same pin configuration. This means that there are no electrical changes when using one or the other.

![Arduino Nano and "KIT DIY 5~7W FM TRANSMITTER" schematic](./schematic_nano.png)


### Regular LCD16x02 (5V) 


![Regular LCD16x02 (5V)](./../../extras/images/lcd16x02_regular_version.jpg)


## Wire up on Arduino UNO, Nano or Pro mini

### LCD 16x2 or 20x4  

  | LCD                       | Device Pin / Label        |  Arduino Pin  |
  | ------------------------- | ------------------------- | ------------  |
  | LCD 16x2 or 20x4          | D4                        |     D7        |
  |                           | D5                        |     D6        |
  |                           | D6                        |     D5        |
  |                           | D7                        |     D4        |
  |                           | RS                        |     D12       |
  |                           | E/ENA                     |     D13       |
  |                           | RW & VSS & K (16)         |    GND        |
  |                           | A (15) & VDD              |    +Vcc       |

### DIY KIT 5˜7W FM TRANSMITTER

  | DIY KIT 5˜7W FM TRANSM.   | Device Pin / Label        |  Arduino Pin  |
  | --------------------------| ------------------------- | --------------|
  | QN8066 BOARD              | SDA  (see *2)             |     A4        |
  |                           | SCLK (see *2)             |     A5        |
  |                           | PWM  (see *1)             |     D9        |

### Buttons 
  
  | Buttons                   | Device Pin / Label        |  Arduino Pin  |
  | ------------------------- | ------------------------- | --------------|
  |                           | Menu                      |      8        |
  |                           | Left (Down / -)           |     10        |
  |                           | Right (Up / + )           |     11        |
  |                           | RESET                     |      3        |


### IMPORTANT

#### (1) Communication Problem Between the KIT and the Microcontroller with PWM

**During various experiments, it was observed that several commands sent to the QN8066 (via I2C) had no effect. For instance, attempts to switch from "Stereo" to "Mono" mode, change the transmission frequency, among other commands, did not alter the transmitter's status. To work around this issue, the PWM must be disabled first, meaning it should not generate a pulse (0% duty cycle). After the command is sent, the PWM signal can be re-enabled. Doing that the system works well.**

#### (2) Compatibility Between the KIT and 5V Microcontroller Such as the Arduino Nano

**There are some important considerations when using a microcontroller like the Arduino Nano with the "DIY 5~7W FM Transmitter Kit". This kit is designed to work with a 3.3V controller, meaning the I2C bus is configured with pull-up resistors connected to the board's power supply. However, the Arduino Nano operates at 5V, producing a 5V signal on the I2C bus, which can cause instability in I2C communication. Additionally, the Arduino Nano will not operate stably when powered by a 3.3V supply (provided by the kit)**.

**The most straightforward and suitable solution, though not very practical, would be to modify the Arduino Nano to operate at 3.3V (by changing the crystal to 8MHz, among other modifications, including changing the bootloader). The most viable solution would be to use an "Arduino Pro Mini 3.3V". This way, no changes or adaptations would be necessary to connect the kit to the microcontroller**.

**For the Arduino Nano, the adaptation used in this example involves using a 3.3V to 5V voltage converter to power the Arduino and placing two 150-ohm resistors in series with the I2C bus and two pull-up 10K resistors connected to the I2C wires.  Although this is not best practice, it works to some extent. If you insist in using a Arduino Nano, a more appropriate solution would be to use a bidirectional I2C level shifter to make the I2C communication compatible with both the Arduino Nano and the kit**.

**Finally, the Arduino Nano was used in this example because it is widely used by most hobbyists. It's likely that many already have one at home, or even an Arduino Uno, which shares the same architecture**.


## Arduino Pro mini schematic 

In this circuit diagram, note that the LCD16x02 is a 3.3V version. You can purchase the LCD in this version or modify a 5V one as shown below.


![Arduino Nano and "KIT DIY 5~7W FM TRANSMITTER" schematic](./schematic_arduino_pro_mini_version.png)


### LCD16x02 3.3V Version

![LCD16x02 3.3V Version - 01](./../../extras/images/lcd16x02_3V3.jpg)

### LCD16x02 5V modified to operate with 3.3V

![LCD16x02 3.3V Version - 01](./../../extras/images/lcd16x02_3V3_modification.jpg)


The IC labeled "7660 AIBAZ DNY17P" is commonly known as the ICL7660. It is a DC-DC voltage converter that functions as a voltage inverter, converting a positive voltage to a negative voltage. This chip is widely used in applications where a negative voltage is needed from a positive power supply. In this case, you use it as a Voltage Doubler. 


  3.3V ---+---[7660]---+----> +6.6V (VOUT)

          |            |

         [C1]         [C2]

          |            |
          
         GND          GND

C1 and C2 are 10µF capacitors.



## Videos related to this example

* [QN8066 Arduino Library by PU2CLR - Part 2](https://youtu.be/3V40yuwN-Vk?si=ZDTcvcty_OK6Nm9N)
* [QN8066 Arduino Library by PU2CLR - Part 1](https://youtu.be/C69MqLq1cw8?si=xd4P6nIfzfCbaN0_)
* [QN8066 RX/TX FM DSP Arduino Library](https://youtu.be/cNQA_2QtBZk?si=Li1ocRp1A5LYIRh8)
