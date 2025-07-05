# Nano 33 IoT as Access Point, PYTHON and QN8066 Controller

This application allows remote control of a transmitter based on the QN8066 through a Python program using a Socket connection. The Arduino sketch for the NANO 33 IoT implements a server that connects to a Wi-Fi network and listens on port 8066. The Python application, in turn, connects to the server or service provided by the NANO 33 IoT and sends configuration commands to the QN8066 transmitter.

## Features

- **Socket Communication**: Direct TCP/IP communication between Python and Arduino
- **Access Point Mode**: Arduino creates its own Wi-Fi network
- **Real-time Control**: Immediate command execution
- **Python Integration**: Complete Python API for transmitter control
- **Standalone Operation**: No external network infrastructure required

## Requirements

### Hardware
- Arduino Nano 33 IoT board
- QN8066 FM transmitter module
- Connecting wires
- Computer with Python 3.x installed

### Software
- Arduino IDE with SAMD board package
- Python 3.x with socket library (built-in)
- Required Arduino libraries:
  - WiFiNINA
  - Wire
  - QN8066

## Hardware Connections

| QN8066 Pin | Nano 33 IoT Pin | Description |
|------------|-----------------|-------------|
| VCC        | 3.3V           | Power supply |
| GND        | GND            | Ground |
| SDA (pin 2)| A4             | I2C Data |
| SCL (pin 1)| A5             | I2C Clock |

## How to Use

### Arduino Setup

1. **Upload Code**: Flash `QN8066_CONTROLLER.ino` to your Nano 33 IoT
2. **Power On**: Connect USB power to the Arduino
3. **Check Serial Monitor**: Verify Access Point started successfully

### Python Setup

1. **Connect to Access Point**: Connect your computer to:
   - **SSID**: Nano33IoT_QN8066
   - **Password**: 12345678
2. **Run Python Script**: Execute the provided Python client

### Access Point Information

- **Network Name (SSID)**: Nano33IoT_QN8066
- **Password**: 12345678
- **Arduino IP**: Use `WiFi.localIP()` (typically 10.0.0.1)
- **Socket Port**: 8066

## Python Client Example

```python
import socket
import time

# Connection settings
HOST = '10.0.0.1'  # Arduino IP address
PORT = 8066         # Socket port

def connect_to_arduino():
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((HOST, PORT))
        return client
    except Exception as e:
        print(f"Connection error: {e}")
        return None

def send_command(client, command):
    try:
        client.send(command.encode('utf-8'))
        response = client.recv(1024).decode('utf-8')
        return response
    except Exception as e:
        print(f"Communication error: {e}")
        return None

# Example usage
if __name__ == "__main__":
    client = connect_to_arduino()
    if client:
        # Set frequency to 106.9 MHz
        response = send_command(client, "FREQ:1069")
        print(f"Frequency response: {response}")
        
        # Set Program Service
        response = send_command(client, "PS:TEST FM")
        print(f"PS response: {response}")
        
        # Set Radio Text
        response = send_command(client, "RT:Arduino Nano 33 IoT Controller")
        print(f"RT response: {response}")
        
        client.close()
```

## Available Commands

The Arduino accepts the following socket commands:

| Command | Format | Description | Example |
|---------|--------|-------------|---------|
| FREQ | FREQ:xxxx | Set frequency (in tenths of MHz) | FREQ:1069 (106.9 MHz) |
| PS | PS:text | Set Program Service (max 8 chars) | PS:TEST FM |
| RT | RT:text | Set Radio Text (max 32 chars) | RT:Hello World |
| POWER | POWER:x | Set power level | POWER:3 |
| STATUS | STATUS | Get current status | STATUS |

## Project Structure

```
NANO_IOT_SOCKET/
├── README.md                    # This file
├── QN8066_CONTROLLER/
│   └── QN8066_CONTROLLER.ino    # Arduino sketch
└── QN8066_PYTHON/
    └── qn8066_client.py         # Python client example
```

## Troubleshooting

### Arduino Issues

1. **Access Point not starting**
   - Check serial monitor for WiFi module detection
   - Verify WiFiNINA library is installed
   - Try resetting the board

2. **Socket connection fails**
   - Ensure port 8066 is not blocked
   - Check if Arduino IP is correct
   - Verify Access Point is running

### Python Issues

1. **Cannot connect to Arduino**
   - Verify connection to Nano33IoT_QN8066 network
   - Check if correct IP address is used
   - Ensure firewall allows socket connections

2. **Commands not working**
   - Check command format (case sensitive)
   - Verify socket connection is established
   - Check Arduino serial monitor for errors

### General Issues

1. **No FM transmission**
   - Verify QN8066 hardware connections
   - Check if frequency is in valid FM band (88-108 MHz)
   - Ensure antenna is connected

## Advanced Usage

### Custom Commands

You can extend the Arduino code to support additional commands:

```cpp
// In Arduino sketch
if (receivedData.startsWith("MYCMD:")) {
    String value = receivedData.substring(6);
    // Process custom command
    client.println("OK");
}
```

### Multiple Clients

The current implementation supports one client at a time. For multiple clients, consider using threading in Python and connection management in Arduino.

## Security Notes

- Default credentials should be changed for production use
- Socket communication is unencrypted
- Access Point has limited range for security

## References

- [Arduino Nano 33 IoT Documentation](https://docs.arduino.cc/hardware/nano-33-iot)
- [Python Socket Programming](https://docs.python.org/3/library/socket.html)
- [QN8066 Library Documentation](../../README.md)
