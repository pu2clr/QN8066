# ESP32 and QN8066 SETUP

ESP32-based boards, with their numerous features such as Bluetooth, WiFi, advanced PWM, Real Time Clock (RTC), and more, offer perhaps the best cost-benefit ratio for developing an application using the QN8066 in an FM transmitter configuration. Leveraging these capabilities, this directory provides several solutions, including an application that utilizes the traditional 16x2 LCD display and rotary encoder, as well as applications that allow the FM transmitter to be controlled via the web or through a local network hosted by the ESP32 itself. Additionally, it is possible to control the transmitter via a Python application using a SOCKET connection. 

The ESP32's signal generation capability can also be used to replace the active crystal required to configure the QN8066. Each subfolder in this directory presents a solution that might be best suited to your needs. 


## ESP32 and Arduino IDE setup

[Installing the ESP32 Board in Arduino IDE (Windows, Mac OS X, Linux)](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).


## ESP32 as a 32.768kHz Oscillator Source

If you don’t have an active crystal available for the QN8066, you can use the ESP32 as an alternative signal generator. In this case, the ESP32 can generate the 32.768kHz signal and feed it directly to the XCLC pin of the QN8066, providing a stable clock source without the need for an external crystal.

This method simplifies the setup while maintaining reliable operation.

To generate a 32.768 kHz signal on a pin of the **ESP32**, you can use the **`ledcWriteTone`** function, which allows generating PWM signals with a specific frequency on any GPIO pin.

Here is an example code to configure **pin 5** of the ESP32 to generate a **32.768 kHz** signal:

```cpp
const int ledcChannel = 0;   // PWM channel (0 to 15)
const int gpioPin = 5;       // Pin for the output signal
const int frequency = 32768; // Frequency in Hz
const int resolution = 8;    // Resolution in bits (can be adjusted as needed)

void setup() {
  // Set up the PWM channel with the desired frequency
  ledcSetup(ledcChannel, frequency, resolution);

  // Attach the PWM channel to the specified pin
  ledcAttachPin(gpioPin, ledcChannel);

  // Start generating the signal on the channel
  ledcWrite(ledcChannel, 128); // Midpoint value for a square wave signal (0-255, where 128 is 50% duty cycle)
}

void loop() {
  // The signal is being continuously generated on the pin
}
```

1. **`ledcSetup`**: Configures the PWM channel with a 32.768 Hz frequency and 8-bit resolution.
2. **`ledcAttachPin`**: Attaches the configured channel to pin 5 (this can be any available GPIO pin).
3. **`ledcWrite`**: Starts generating the PWM signal with a 50% duty cycle (value 128 in 8-bit resolution).

This code will generate a 32.768 kHz signal on the defined pin. Adjust the pin and resolution as needed. You can adapt it to ESP32 and QN8066 application. 
