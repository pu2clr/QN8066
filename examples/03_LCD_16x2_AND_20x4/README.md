# QN8066 Arduino Library and LCD 16x02 examples

![UNDER CONSTRUCTION...](../../extras/images/under_construction.png)


## Arduino Nano and "KIT DIY 5~7W FM TRANSMITTER" schematic

The following schematic illustrates the connections between the Arduino Nano, the "KIT DIY 5~7W FM TRANSMITTER," and either the LCD 16x2 or the LCD 20x4. It is important to note that both the LCD 16x2 and the LCD 20x4 have the same pin configuration. This means that there are no electrical changes when using one or the other.

![Arduino Nano and "KIT DIY 5~7W FM TRANSMITTER" schematic](./schematic.png)


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
  | QN8066 BOARD              | SDA                       |     A4        |
  |                           | SCLK                      |     A5        |
  |                           | PWM                       |     D9        |

### Buttons 
  
  | Buttons                   | Device Pin / Label        |  Arduino Pin  |
  | ------------------------- | ------------------------- | --------------|
  |                           | Menu                      |      8        |
  |                           | Left (Down / -)           |     10        |
  |                           | Right (Up / + )           |     11        |
  |                           | RESET                     |      3        |



### IMPORTANT

**There are some important considerations when using a microcontroller like the Arduino Nano with the "DIY 5~7W FM Transmitter Kit". This kit is designed to work with a 3.3V controller, meaning the I2C bus is configured with pull-up resistors connected to the board's power supply. However, the Arduino Nano operates at 5V, producing a 5V signal on the I2C bus, which can cause instability in I2C communication. Additionally, the Arduino Nano will not operate stably when powered by a 3.7V supply (provided by the kit)**.

**The most straightforward and suitable solution, though not very practical, would be to modify the Arduino Nano to operate at 3.3V (by changing the crystal to 8MHz, among other modifications, including changing the bootloader). The most viable solution would be to use an "Arduino Pro Mini 3.3V". This way, no changes or adaptations would be necessary to connect the kit to the microcontroller**.

**For the Arduino Nano, the adaptation used in this example involves using a 3.3V to 5V voltage converter to power the Arduino and placing two 150-ohm resistors in series with the I2C bus. Although this is not best practice, it works to some extent. If you insist in using a Arduino Nano, a more appropriate solution would be to use a bidirectional I2C level shifter to make the I2C communication compatible with both the Arduino Nano and the kit**.

**Finally, the Arduino Nano was used in this example because it is widely used by most hobbyists. It's likely that many already have one at home, or even an Arduino Uno, which shares the same architecture**.