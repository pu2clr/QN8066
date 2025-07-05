#!/usr/bin/env python3
"""
QN8066 Controller - Python Client for Arduino Nano 33 IoT

This Python script demonstrates how to control a QN8066 FM transmitter
connected to an Arduino Nano 33 IoT through a socket connection.

Requirements:
1. Arduino Nano 33 IoT running QN8066_CONTROLLER.ino
2. Computer connected to "Nano33IoT_QN8066" WiFi network
3. Python 3.x (socket library is built-in)

Usage:
    python qn8066_client.py

Author: PU2CLR
Date: July 2025
"""

import socket
import time
import sys

# Connection settings
HOST = '10.0.0.1'  # Arduino Nano 33 IoT IP address (check serial monitor)
PORT = 8066         # Socket port defined in Arduino sketch
TIMEOUT = 10        # Connection timeout in seconds

class QN8066Controller:
    """
    Python client for controlling QN8066 FM transmitter through Arduino Nano 33 IoT
    """
    
    def __init__(self, host=HOST, port=PORT, timeout=TIMEOUT):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.client = None
        self.connected = False
    
    def connect(self):
        """
        Establish socket connection to Arduino
        Returns: True if successful, False otherwise
        """
        try:
            print(f"Connecting to Arduino at {self.host}:{self.port}...")
            self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client.settimeout(self.timeout)
            self.client.connect((self.host, self.port))
            self.connected = True
            print("✓ Connected successfully!")
            return True
        except socket.timeout:
            print("✗ Connection timeout. Check if Arduino is running and accessible.")
            return False
        except ConnectionRefusedError:
            print("✗ Connection refused. Check if Arduino server is running.")
            return False
        except Exception as e:
            print(f"✗ Connection error: {e}")
            return False
    
    def disconnect(self):
        """
        Close socket connection
        """
        if self.client:
            self.client.close()
            self.connected = False
            print("Disconnected from Arduino")
    
    def send_command(self, command):
        """
        Send command to Arduino and receive response
        Args:
            command (str): Command to send
        Returns:
            str: Response from Arduino, or None if error
        """
        if not self.connected:
            print("✗ Not connected to Arduino")
            return None
        
        try:
            # Send command
            self.client.send(command.encode('utf-8'))
            print(f"→ Sent: {command}")
            
            # Receive response
            response = self.client.recv(1024).decode('utf-8').strip()
            print(f"← Received: {response}")
            return response
        except socket.timeout:
            print("✗ Command timeout")
            return None
        except Exception as e:
            print(f"✗ Communication error: {e}")
            return None
    
    def set_frequency(self, freq_mhz):
        """
        Set transmission frequency
        Args:
            freq_mhz (float): Frequency in MHz (88.0 - 108.0)
        Returns:
            str: Response from Arduino
        """
        if not (88.0 <= freq_mhz <= 108.0):
            print("✗ Frequency must be between 88.0 and 108.0 MHz")
            return None
        
        freq_tenths = int(freq_mhz * 10)
        return self.send_command(f"FREQ:{freq_tenths}")
    
    def set_program_service(self, ps_text):
        """
        Set Program Service (station identifier)
        Args:
            ps_text (str): Program Service text (max 8 characters)
        Returns:
            str: Response from Arduino
        """
        if len(ps_text) > 8:
            print("✗ Program Service text must be 8 characters or less")
            return None
        
        return self.send_command(f"PS:{ps_text}")
    
    def set_radio_text(self, rt_text):
        """
        Set Radio Text (scrolling message)
        Args:
            rt_text (str): Radio Text (max 32 characters)
        Returns:
            str: Response from Arduino
        """
        if len(rt_text) > 32:
            print("✗ Radio Text must be 32 characters or less")
            return None
        
        return self.send_command(f"RT:{rt_text}")
    
    def set_power(self, power_level):
        """
        Set transmission power level
        Args:
            power_level (int): Power level (0-3)
        Returns:
            str: Response from Arduino
        """
        if not (0 <= power_level <= 3):
            print("✗ Power level must be between 0 and 3")
            return None
        
        return self.send_command(f"POWER:{power_level}")
    
    def get_status(self):
        """
        Get current transmitter status
        Returns:
            str: Status information from Arduino
        """
        return self.send_command("STATUS")

def interactive_mode(controller):
    """
    Interactive command-line interface
    """
    print("\n" + "="*50)
    print("QN8066 Interactive Control Mode")
    print("="*50)
    print("Commands:")
    print("  freq <MHz>     - Set frequency (88.0-108.0)")
    print("  ps <text>      - Set Program Service (max 8 chars)")
    print("  rt <text>      - Set Radio Text (max 32 chars)")
    print("  power <0-3>    - Set power level")
    print("  status         - Get current status")
    print("  quit           - Exit program")
    print("="*50)
    
    while True:
        try:
            cmd = input("\nQN8066> ").strip().lower()
            
            if cmd == "quit" or cmd == "exit":
                break
            elif cmd == "status":
                controller.get_status()
            elif cmd.startswith("freq "):
                try:
                    freq = float(cmd.split()[1])
                    controller.set_frequency(freq)
                except (IndexError, ValueError):
                    print("✗ Usage: freq <MHz> (e.g., freq 106.9)")
            elif cmd.startswith("ps "):
                try:
                    ps_text = cmd.split(" ", 1)[1]
                    controller.set_program_service(ps_text)
                except IndexError:
                    print("✗ Usage: ps <text> (e.g., ps TEST FM)")
            elif cmd.startswith("rt "):
                try:
                    rt_text = cmd.split(" ", 1)[1]
                    controller.set_radio_text(rt_text)
                except IndexError:
                    print("✗ Usage: rt <text> (e.g., rt Hello World)")
            elif cmd.startswith("power "):
                try:
                    power = int(cmd.split()[1])
                    controller.set_power(power)
                except (IndexError, ValueError):
                    print("✗ Usage: power <0-3> (e.g., power 2)")
            elif cmd == "help" or cmd == "?":
                print("See command list above")
            elif cmd == "":
                continue
            else:
                print("✗ Unknown command. Type 'help' for available commands.")
                
        except KeyboardInterrupt:
            print("\nExiting...")
            break
        except EOFError:
            break

def demo_mode(controller):
    """
    Demonstration of various transmitter controls
    """
    print("\n" + "="*50)
    print("QN8066 Demo Mode")
    print("="*50)
    
    # Demo sequence
    commands = [
        ("Set frequency to 106.9 MHz", lambda: controller.set_frequency(106.9)),
        ("Set Program Service to 'ARDUINO'", lambda: controller.set_program_service("ARDUINO")),
        ("Set Radio Text to 'Nano 33 IoT Controller'", lambda: controller.set_radio_text("Nano 33 IoT Controller")),
        ("Set power level to 2", lambda: controller.set_power(2)),
        ("Get current status", lambda: controller.get_status()),
    ]
    
    for description, command in commands:
        print(f"\n{description}...")
        time.sleep(1)
        command()
        time.sleep(2)
    
    print("\n✓ Demo completed!")

def main():
    """
    Main program entry point
    """
    print("QN8066 Controller - Python Client")
    print("Connecting to Arduino Nano 33 IoT...")
    print("\nMake sure you are connected to 'Nano33IoT_QN8066' WiFi network!")
    
    # Create controller instance
    controller = QN8066Controller()
    
    # Connect to Arduino
    if not controller.connect():
        print("\nTroubleshooting steps:")
        print("1. Check if Arduino is powered and running QN8066_CONTROLLER.ino")
        print("2. Verify connection to 'Nano33IoT_QN8066' WiFi network")
        print("3. Check if IP address is correct (check Arduino serial monitor)")
        print("4. Ensure firewall allows socket connections on port 8066")
        sys.exit(1)
    
    try:
        # Get initial status
        print("\nGetting initial status...")
        controller.get_status()
        
        # Ask user for mode
        print("\nSelect mode:")
        print("1. Interactive mode (manual control)")
        print("2. Demo mode (automated demonstration)")
        
        while True:
            try:
                choice = input("\nEnter choice (1 or 2): ").strip()
                if choice == "1":
                    interactive_mode(controller)
                    break
                elif choice == "2":
                    demo_mode(controller)
                    break
                else:
                    print("Please enter 1 or 2")
            except KeyboardInterrupt:
                print("\nExiting...")
                break
    
    finally:
        # Clean up
        controller.disconnect()

if __name__ == "__main__":
    main()
