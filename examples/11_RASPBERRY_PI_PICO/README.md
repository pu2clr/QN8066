# QN8066 Arduino Library Examples for Raspberry Pi Pico

This directory contains examples specifically designed for the Raspberry Pi Pico microcontroller and the QN8066 FM transceiver. The Raspberry Pi Pico is an excellent choice for QN8066 projects due to its 3.3V operation, dual-core ARM Cortex-M0+ processor, and abundant GPIO pins.

## Examples Available

### 1. BASIC_TX_PICO - Basic FM Transmitter
A simple FM transmitter example that demonstrates the fundamental features of the QN8066 with minimal hardware requirements.

**Features:**
- Basic FM transmission on a specified frequency
- Stereo audio capability  
- Real-time status monitoring via Serial Monitor
- Simple hardware setup with minimal components

**Hardware Required:**
- Raspberry Pi Pico
- QN8066 module
- 32.768 kHz active crystal oscillator
- Antenna (or dummy load for testing)

**Ideal for:** Beginners, quick testing, minimal setups

### 2. PICO_LCD16x2_ENCODER - Advanced Control System
A complete FM transmitter control system with LCD display and rotary encoder for interactive operation.

**Features:**
- LCD 16x2 display showing frequency, power, and status
- Rotary encoder for parameter adjustment
- Push button for menu navigation
- Multiple operating modes (Frequency, Power, Stereo/Mono)
- Real-time audio level monitoring
- Professional user interface

**Hardware Required:**
- Raspberry Pi Pico
- QN8066 module
- LCD 16x2 with I2C backpack
- Rotary encoder with push button
- 32.768 kHz active crystal oscillator
- Antenna (or dummy load for testing)

**Ideal for:** Advanced users, permanent installations, professional applications

## Why Raspberry Pi Pico?

The Raspberry Pi Pico is particularly well-suited for QN8066 projects because:

### Advantages:
- **3.3V Native Operation**: Perfect compatibility with QN8066's voltage requirements
- **Abundant GPIO**: 26 GPIO pins for multiple peripherals
- **Dual-Core Processor**: RP2040 with 133MHz ARM Cortex-M0+ cores
- **Flexible I2C**: Hardware I2C with configurable pins
- **Cost-Effective**: Low cost compared to other 32-bit microcontrollers
- **Arduino IDE Support**: Easy programming with familiar environment
- **Rich Ecosystem**: Large community and extensive library support

### Technical Specifications:
- **Microcontroller**: RP2040 dual-core ARM Cortex-M0+
- **Operating Voltage**: 3.3V (perfect for QN8066)
- **Flash Memory**: 2MB
- **SRAM**: 264KB
- **GPIO Pins**: 26 (including I2C, SPI, UART)
- **ADC**: 12-bit, 4 channels
- **PWM**: 16 channels
- **Clock Speed**: Up to 133MHz

## Hardware Setup Guide

### Basic Connections (Both Examples)

| Raspberry Pi Pico | QN8066 | Description |
|-------------------|---------|-------------|
| 3.3V (Pin 36)     | VCC     | Power supply |
| GND (Pin 38)      | GND     | Ground |
| GP0 (Pin 1)       | SDA     | I2C Data |
| GP1 (Pin 2)       | SCL     | I2C Clock |
| GP16 (Pin 21)     | CEN     | Chip Enable (Optional) |

### External Components:
- **Active Crystal**: 32.768 kHz → QN8066 XCLK (Pin 14)
- **Antenna**: Properly matched → QN8066 RFO (Pin 12)
- **Audio Input**: Audio source → QN8066 ALI/ARI (Pins 10/9)

## Software Requirements

### Arduino IDE Setup:
1. **Install Pico Support**: Add board manager URL for RP2040
2. **Install QN8066 Library**: Available in Library Manager
3. **Select Board**: "Raspberry Pi Pico" under RP2040 boards
4. **Configure Flash**: Minimum 2MB recommended

### Required Libraries:
- **QN8066** - by PU2CLR (for both examples)
- **LiquidCrystal_I2C** - by Frank de Brabander (for LCD example only)

## Getting Started

### For Beginners:
1. Start with **BASIC_TX_PICO** example
2. Build the minimal circuit
3. Upload and test basic functionality
4. Experiment with frequency and power settings

### For Advanced Users:
1. Build the **PICO_LCD16x2_ENCODER** circuit
2. Install required libraries
3. Upload and enjoy the full-featured interface
4. Customize for your specific needs

## Important Notes

### Legal Compliance:
- **Always comply with local FM transmission regulations**
- Use appropriate frequency bands for your region
- Consider transmission power limits
- Use dummy loads for testing to avoid interference

### Technical Considerations:
- **Active Crystal Required**: QN8066 needs active oscillator, not passive crystal
- **Proper Grounding**: Ensure good ground connections for stability
- **I2C Pull-ups**: Usually built into modules, but check if needed
- **Antenna Matching**: Use properly matched antenna for best performance

### Safety:
- **3.3V Operation**: Pico's 3.3V is perfect for QN8066
- **Current Limits**: Don't exceed GPIO current ratings
- **RF Safety**: Use appropriate shielding and grounding

## Troubleshooting

### Common Issues:

**Device Not Detected:**
- Check I2C connections (SDA/SCL)
- Verify power supply (3.3V)
- Ensure active crystal is connected and working

**No Audio Output:**
- Check audio connections to ALI/ARI pins
- Verify audio source levels
- Test with simple audio input (smartphone, etc.)

**LCD Issues (Advanced Example):**
- Check LCD I2C address (0x27 or 0x3F)
- Verify LCD power connections
- Test I2C bus with scanner sketch

**Encoder Issues (Advanced Example):**
- Check encoder connections and orientation
- Verify pull-up resistors on encoder pins
- Test encoder mechanically before assuming software issues

## Performance Tips

### Optimization:
- **Use Core 0 for I2C**: Keep real-time operations on one core
- **Minimize Interrupts**: Debounce encoder properly
- **Efficient Display Updates**: Only update LCD when values change
- **Serial Monitor**: Use for debugging but disable for production

### Power Management:
- **Sleep Modes**: Use Pico's sleep features for battery operation
- **Clock Management**: Reduce clock speed if not needed
- **Peripheral Control**: Disable unused peripherals

## Expansion Ideas

These examples can be extended with:

### Hardware Additions:
- **OLED Display**: Replace LCD with OLED for better visibility
- **Multiple Encoders**: Separate controls for different parameters
- **LED Indicators**: Status LEDs for quick visual feedback
- **Audio Processing**: Add audio effects or level control
- **Remote Control**: IR receiver for wireless control

### Software Features:
- **RDS Support**: Add station name and text transmission
- **Frequency Presets**: Store and recall favorite frequencies
- **Audio Level Meters**: Real-time VU meters on display
- **Web Interface**: Use Pico W for network control
- **Data Logging**: Log transmission parameters to flash memory

### Advanced Projects:
- **Multi-Channel TX**: Control multiple QN8066 modules
- **Audio Streaming**: Network audio streaming to FM
- **Remote Monitoring**: Send status via WiFi (Pico W)
- **Voice Control**: Add speech recognition
- **Automated DJ**: Playlist management and scheduling

## Support and Documentation

### Resources:
- **QN8066 Library Documentation**: https://pu2clr.github.io/QN8066/
- **GitHub Repository**: https://github.com/pu2clr/QN8066
- **Raspberry Pi Pico Documentation**: https://www.raspberrypi.org/documentation/pico/
- **Arduino IDE Support**: https://github.com/earlephilhower/arduino-pico

### Community:
- **Arduino Forum**: arduino.cc/forum
- **Reddit**: r/arduino, r/raspberry_pi
- **Discord**: Various Arduino and Raspberry Pi servers

## License

These examples are part of the QN8066 Arduino Library, distributed under the MIT License. You are free to use, modify, and distribute these examples for both personal and commercial projects.
