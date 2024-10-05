# ESP32, QN8066, LCD16x2 and Encoder basic setup

Before starting compiling ESP32 based sketches, you must configure your Arduino IDE. Follow the steps below:

* Install the last version of Arduino IDE
* Configure the ESP32 on Arduino IDE
  * Open the Arduino IDE and select the __Preferences...__ menu item.
  * Go to __Additional Boards Manager URLs:__ and add the URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  * After adding the ESP32 URL, go to __Tools__ menu, select the __Boards__ item and then select __Boards Manager...__ option. Finally install ESP32 board.

## ESP32 Dev Module setup

Observe in the following diagram and table that the connections between the ESP32 and the QN8066, as well as the encoder and the LCD, refer to the GPIO and not to the physical pin number of the ESP32 module board. Carefully analyze the pin distribution of your ESP32 to make the correct connections


![ESP32, QN8066, LCD16x2 and Encoder basic setup](./schematic_esp32_qn8066_encoder_lcd16x2.png)


## Wire up ESP32 Dev Module, QN8066 and LCD16x02 or LCD16x04

| Device name               | Device Pin / Description  |  Arduino Pin  |
| --------------------------| --------------------      | ------------  |
|    LCD 16x2 or 20x4       |                           |               |
|                           | D4                        |  GPIO18       |
|                           | D5                        |  GPIO17       |
|                           | D6                        |  GPIO16       |
|                           | D7                        |  GPIO15       |
|                           | RS                        |  GPIO19       |
|                           | E/ENA                     |  GPIO23       |
|                           | RW & VSS & K (16)         |  GND          |
|                           | A (15) & VDD              |  +Vcc         |
| --------------------------| ------------------------- | --------------|
| QN8066                    |                           |               | 
|                           | VCC                       |  3.3V         |
|                           | SDIO / SDA (pin 2)        |  GPIO21 [1]   |
|                           | SCLK (pin 1)              |  GPIO22 [1]   |
| --------------------------| --------------------------| --------------|
| Encoder                   |                           |               |
|                           | A                         |  GPIO13       |
|                           | B                         |  GPIO14       |
|                           | PUSH BUTTON (encoder)     |  GPIO27       |
| PWM                       |                           |               |
|                           |                           |  GPIO12       | 

1. It can change if you are not using the ESP32 Dev Module. Check you ESP32 board pinout 



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

### Explanation:
1. **`ledcSetup`**: Configures the PWM channel with a 32.768 Hz frequency and 8-bit resolution.
2. **`ledcAttachPin`**: Attaches the configured channel to pin 5 (this can be any available GPIO pin).
3. **`ledcWrite`**: Starts generating the PWM signal with a 50% duty cycle (value 128 in 8-bit resolution).

This code will generate a 32.768 kHz signal on the defined pin. Adjust the pin and resolution as needed.
