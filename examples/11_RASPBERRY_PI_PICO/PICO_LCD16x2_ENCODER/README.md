# QN8066 with Raspberry Pi Pico, LCD16x2 and Rotary Encoder

This example demonstrates a complete FM transmitter control system using:
- Raspberry Pi Pico as the main controller
- QN8066 FM transceiver
- 16x2 LCD display with I2C backpack
- Rotary encoder with push button for navigation

## Circuit Diagram

```
                    Raspberry Pi Pico
                   ┌───────────────────┐
                   │                   │
             ┌─────┤ 3.3V (36)         │
             │     │                   │
     ┌───────┼─────┤ GND (38)          │
     │       │     │                   │
     │   ┌───┼─────┤ GP0 (1)  SDA      │ ← I2C Bus
     │   │   │     │                   │
     │   │   │ ┌───┤ GP1 (2)  SCL      │ ← I2C Bus
     │   │   │ │   │                   │
     │   │   │ │ ┌─┤ GP2 (4)  ENC_A    │ ← Encoder A
     │   │   │ │ │ │                   │
     │   │   │ │ │ ┌┤ GP3 (5)  ENC_B   │ ← Encoder B
     │   │   │ │ │ ││                  │
     │   │   │ │ │ │┌┤ GP4 (6)  ENC_BTN│ ← Encoder Button
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │││ GP16(21) CEN    │ ← QN8066 Enable
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │││                 │
     │   │   │ │ │ │├┘                 │
     │   │   │ │ │ ├┘                  │
     │   │   │ │ ├┘                    │
     │   │   │ ├┘                      │
     │   │   ├┘                        │
     │   │   │                         │
     │   │   │                         │
     │   │   │    ┌─────────────┐      │
     │   │   └────┤ SDA    VCC  ├──────┘
     │   │        │             │
     │   └────────┤ SCL    GND  ├──┐
     │            │             │  │
     │            │    LCD16x2  │  │
     │            │   I2C 0x27  │  │
     │            └─────────────┘  │
     │                             │
     │                             │
     │        ┌─────────────┐      │
     │   ┌────┤ SDA    VCC  ├──────┼──┘
     │   │    │             │      │
     │   │ ┌──┤ SCL    GND  ├──────┼──┐
     │   │ │  │             │      │  │
     │   │ │  │   QN8066    │      │  │
     │   │ │  │             │      │  │
     │   │ │  │ CEN   XCLK  ├──────┼──┼──── 32.768kHz
     │   │ │  │             │      │  │     Crystal
     │   │ │  │ ALI    RFO  ├──────┼──┼──── Antenna
     │   │ │  │             │      │  │
     │   │ │  │ ARI         │      │  │
     │   │ │  └─────────────┘      │  │
     │   │ │                       │  │
     │   │ └───────────────────────┘  │
     │   └───────────────────────────┼──┘
     │                               │
     │                               │
     │   ┌─────────────────────┐     │
     │   │    Rotary Encoder   │     │
     │   │                     │     │
     ├───┤ A                   │     │
     │   │                     │     │
     ├───┤ B                   │     │
     │   │                     │     │
     ├───┤ BTN                 │     │
     │   │                     │     │
     └───┤ GND                 │     │
         │                     │     │
         └─────────────────────┘     │
                                     │
     ┌───────────────────────────────┘
     │
     │   Common Ground
     └───────────────────────────────────
```

## Hardware Components

### Required Components:
1. **Raspberry Pi Pico** - Main microcontroller
2. **QN8066 Module** - FM transceiver
3. **LCD 16x2 with I2C backpack** - Display (address usually 0x27)
4. **Rotary Encoder with push button** - User interface
5. **32.768 kHz Active Crystal** - Clock reference
6. **Breadboard and jumper wires** - Connections

### Optional Components:
- **Audio source** - Connect to ALI/ARI pins
- **Antenna or dummy load** - Connect to RFO pin
- **Pull-up resistors** - 4.7kΩ for I2C (usually built into modules)

## Pin Assignments

| Raspberry Pi Pico | Component | Pin/Signal | Description |
|-------------------|-----------|------------|-------------|
| 3.3V (Pin 36) | All modules | VCC | Power supply |
| GND (Pin 38) | All modules | GND | Common ground |
| GP0 (Pin 1) | I2C Bus | SDA | I2C Data |
| GP1 (Pin 2) | I2C Bus | SCL | I2C Clock |
| GP2 (Pin 4) | Encoder | A | Encoder channel A |
| GP3 (Pin 5) | Encoder | B | Encoder channel B |
| GP4 (Pin 6) | Encoder | Button | Push button |
| GP16 (Pin 21) | QN8066 | CEN | Chip enable (optional) |

## Software Features

### Display Information:
- **Line 1**: Current frequency (MHz) and stereo/mono indicator
- **Line 2**: Transmission power and current menu mode
- **Menu indicator**: Shows current selection mode (FRQ/PWR/MOD)

### User Interface:
- **Rotate encoder**: Adjust current parameter
- **Press button**: Switch between modes (Frequency → Power → Mode → Frequency...)
- **Serial monitor**: Detailed status information

### Operating Modes:
1. **FRQ Mode**: Adjust transmission frequency (87.5 - 108.0 MHz)
2. **PWR Mode**: Adjust transmission power (20 - 120)
3. **MOD Mode**: Toggle between Stereo and Mono

## Libraries Required

Install these libraries through Arduino IDE Library Manager:

1. **QN8066** - by PU2CLR
2. **LiquidCrystal I2C** - by Frank de Brabander

## Usage Instructions

1. **Upload the sketch** to your Raspberry Pi Pico
2. **Open Serial Monitor** (115200 baud) for debugging
3. **Use the encoder** to navigate:
   - Turn to adjust current parameter
   - Press to switch between modes
4. **Monitor the LCD** for real-time information
5. **Check serial output** for detailed status

## Troubleshooting

### LCD Issues:
- **Blank display**: Check I2C address (try 0x3F instead of 0x27)
- **Garbled text**: Check I2C connections and power
- **No backlight**: Verify LCD I2C module power

### Encoder Issues:
- **No response**: Check pin connections and pull-up resistors
- **Erratic behavior**: Add debouncing delay or hardware debouncing
- **Button not working**: Verify button pin and interrupt setup

### QN8066 Issues:
- **Device not detected**: Check I2C connections and crystal
- **No transmission**: Verify frequency range and power settings
- **Poor audio**: Check audio input connections and levels

## Advanced Features

This example can be extended with:
- **RDS transmission** - Add station name and text
- **Frequency presets** - Store favorite frequencies
- **Audio level meters** - Real-time audio monitoring
- **Web interface** - Use Pico W for remote control
- **EEPROM storage** - Save settings between power cycles

## Serial Monitor Output Example

```
QN8066 Pico LCD Control Starting...
System ready!
Freq: 106.9 MHz | Power: 73 | Mode: Stereo | FSM: 10 | Audio: 45 mV
Freq: 107.5 MHz | Power: 73 | Mode: Stereo | FSM: 10 | Audio: 52 mV
Freq: 107.5 MHz | Power: 83 | Mode: Stereo | FSM: 10 | Audio: 52 mV
```

This example provides a complete, interactive FM transmitter control system with professional-grade user interface and monitoring capabilities.
