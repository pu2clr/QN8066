# Nano 33 IoT as Access Point, WEB Service and QN8066 Controller

This application demonstrates how to use the Arduino Nano 33 IoT as an Access Point to control a QN8066 FM transmitter through a web interface. The Nano 33 IoT creates its own Wi-Fi network, allowing direct connection from any device with Wi-Fi capability for remote FM transmitter configuration.

## Features

- **Access Point Mode**: Creates a standalone Wi-Fi network
- **Web Interface**: User-friendly HTML interface for transmitter control
- **Real-time Control**: Immediate parameter updates
- **RDS Support**: Program Service (PS) and Radio Text (RT) configuration
- **No External Network Required**: Complete standalone operation

## Requirements

### Hardware
- Arduino Nano 33 IoT board
- QN8066 FM transmitter module
- Connecting wires
- Optional: External antenna for better transmission

### Software
- Arduino IDE with SAMD board package installed
- Required libraries:
  - WiFiNINA (included with SAMD board package)
  - Wire (Arduino core library)
  - QN8066 (this library)

## Hardware Connections

| QN8066 Pin | Nano 33 IoT Pin | Description |
|------------|-----------------|-------------|
| VCC        | 3.3V           | Power supply |
| GND        | GND            | Ground |
| SDA (pin 2)| A4             | I2C Data |
| SCL (pin 1)| A5             | I2C Clock |

## How to Use

### Setup Instructions

1. **Hardware Connection**: Wire the Nano 33 IoT and QN8066 according to the table above
2. **Upload Code**: Flash the appropriate sketch to your Nano 33 IoT
3. **Power On**: Connect USB power to the Nano 33 IoT
4. **Connect to Access Point**: On your device, connect to:
   - **SSID**: QN8066
   - **Password**: qn8066123456
5. **Access Web Interface**: Open browser and navigate to **10.0.0.1**
6. **Configure**: Use the web interface to control the FM transmitter

### Access Point Information

- **Network Name (SSID)**: QN8066
- **Password**: qn8066123456
- **IP Address**: 10.0.0.1
- **Subnet Mask**: 255.255.255.0

### Available Controls

- **Frequency**: Set transmission frequency (88.0 - 108.0 MHz)
- **Program Service (PS)**: 8-character station identifier
- **Radio Text (RT)**: 64-character scrolling message
- **Power Level**: Transmission power control (if supported)

## Project Structure

```
NANO_IOT_WEB_SERVICE/
├── README.md                    # This file
└── NANO_CAPTIVE_PORTAL/
    └── NANO_CAPTIVE_PORTAL.ino  # Main sketch file
```

## Troubleshooting

### Common Issues

1. **Cannot see QN8066 network**
   - Check if Nano 33 IoT is powered on
   - Verify sketch uploaded successfully
   - Check serial monitor for startup messages

2. **Cannot connect to 10.0.0.1**
   - Ensure connected to QN8066 network (not your home Wi-Fi)
   - Try refreshing the browser
   - Check if firewall is blocking connection

3. **No FM transmission**
   - Verify QN8066 connections
   - Check if antenna is connected
   - Ensure frequency is set correctly

4. **Web interface not responding**
   - Check serial monitor for error messages
   - Try resetting the Nano 33 IoT
   - Verify all required libraries are installed

### LED Indicators

- **Power LED**: Should be solid when powered
- **WiFi LED**: May blink during setup, solid when Access Point is active

## Example Usage

1. Power on the Nano 33 IoT
2. Wait for the Access Point to start (check serial monitor)
3. Connect your phone/computer to "QN8066" network
4. Open browser and go to 10.0.0.1
5. Set frequency to 106.9 MHz
6. Enter "TEST FM" as Program Service
7. Tune your FM radio to 106.9 MHz to hear the transmission

## Security Notes

- Default password should be changed for production use
- Access Point has limited range (typically 10-50 meters)
- No internet connectivity is provided through this Access Point

## References

- [Arduino Nano 33 IoT Documentation](https://docs.arduino.cc/hardware/nano-33-iot)
- [WiFiNINA Library Reference](https://www.arduino.cc/reference/en/libraries/wifinina/)
- [QN8066 Library Documentation](../../README.md)
