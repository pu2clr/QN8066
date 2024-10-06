# ESP32 and QN8066 SETUP

ESP32-based boards, with their numerous features such as Bluetooth, WiFi, advanced PWM, Real Time Clock (RTC), and more, offer perhaps the best cost-benefit ratio for developing an application using the QN8066 in an FM transmitter configuration. Leveraging these capabilities, this directory provides several solutions, including an application that utilizes the traditional 16x2 LCD display and rotary encoder, as well as applications that allow the FM transmitter to be controlled via the web or through a local network hosted by the ESP32 itself. Additionally, it is possible to control the transmitter via a Python application using a SOCKET connection. 

The ESP32's signal generation capability can also be used to replace the active crystal required to configure the QN8066. Each subfolder in this directory presents a solution that might be best suited to your needs. 


## ESP32 and Arduino IDE setup

[Installing the ESP32 Board in Arduino IDE (Windows, Mac OS X, Linux)](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).


## Some ESP32 Tips to be tested


### ESP32 as a 32.768kHz Oscillator Source

If you don’t have an active crystal available for the QN8066, you can try to use the ESP32 as an alternative signal generator. In this case, the ESP32 can generate the 32.768kHz signal and feed it directly to the XCLC pin of the QN8066, providing a stable clock source without the need for an external crystal.

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


### ESP32 dual core feature

Monitoring the rotary encoder and buttons, while still using the ESP32's interrupt feature, is very efficient, and the microcontroller is much faster compared to the Arduino based on the ATMega328. However, there might still be some delay after the user's action. I would like to point out that this does not seem to be an issue, as this delay is minimal even in microcontrollers with lower speeds. Nevertheless, the ESP32 has two processing cores, allowing parallel processing.

Since the ESP32 is dual-core, you can dedicate one core exclusively to the task of intensive communication over I2C, while the other core handles user input events. This way, the system becomes more responsive because operations that don't depend on I2C (like monitoring buttons and encoders) will be processed by the other core.

#### Implementing the Solution

You can implement an architecture where one task manages the I2C on Core 1, and another task is dedicated to listening for button and encoder inputs on Core 0. See the code below.



```cpp
#include <Wire.h>

TaskHandle_t TaskI2C;
TaskHandle_t TaskUserInput;

// Variables to simulate buttons and encoder
volatile int buttonPressed = 0;
volatile int encoderValue = 0;

// Function for the I2C Task on Core 1
void TaskI2Ccode( void * parameter )
{
  for(;;)
  {
    // Perform intensive I2C communications
    Wire.beginTransmission(0x3C);
    Wire.write(0x00); // Simulate a command
    Wire.endTransmission();

    delay(50); // Adjust as necessary to simulate I2C load
  }
}

// Function for User Interaction Task on Core 0
void TaskUserInputCode( void * parameter )
{
  for(;;)
  {
    // Simulate button check
    if (buttonPressed)
    {
      // Process the button action
      buttonPressed = 0;
    }

    // Simulate encoder check
    if (encoderValue != 0)
    {
      // Process the encoder value
      encoderValue = 0;
    }

    delay(10); // Small delay to avoid overloading the core
  }
}

void setup() {
  // Initialize the I2C interface
  Wire.begin();

  // Create the I2C Task on Core 1
  xTaskCreatePinnedToCore(
    TaskI2Ccode,      // Function to execute
    "TaskI2C",        // Task name
    10000,            // Stack size
    NULL,             // Parameter passed to function
    1,                // Task priority
    &TaskI2C,         // Task handle
    1);               // Core 1

  // Create the User Input Task on Core 0
  xTaskCreatePinnedToCore(
    TaskUserInputCode, // Function to execute
    "TaskUserInput",   // Task name
    10000,             // Stack size
    NULL,              // Parameter passed to function
    1,                 // Task priority
    &TaskUserInput,    // Task handle
    0);                // Core 0
}

void loop() {
  // Main loop can remain empty or perform other tasks
}
```

* Core 1 is responsible for all I2C operations, managing communication with the bus intensively without affecting system responsiveness.
* Core 0 handles button and encoder inputs, ensuring that the system continues to respond quickly to user commands.

* Task priorities: If you find that the system is still not as responsive as you'd like, you can adjust task priorities, giving higher priority to the task managing the buttons and encoder.
* I2C mutex: If the buttons or encoder need to interact with the I2C at any point, implement a mutex (as mentioned in the previous solution) to avoid conflicts on the bus.




