# NANO 33 IOT and QN8066 setup

The [Arduino Nano 33 IoT](https://docs.arduino.cc/resources/datasheets/ABX00027-datasheet.pdf) is a compact board that offers Wi-Fi and Bluetooth connectivity, making it an excellent choice for Internet of Things (IoT) projects. It is designed as an evolution of the Nano series, adding wireless connectivity while maintaining a simple and accessible interface.

### Key Features:
1. **Microcontroller**: SAMD21 Cortex®-M0+ 32-bit ARM®.
2. **Connectivity**:
   - **Wi-Fi**: NINA-W102 module (IEEE 802.11b/g/n).
   - **Bluetooth 4.2**: Also integrated into the NINA-W102 module.
3. **Memory**:
   - **Flash**: 256 KB.
   - **SRAM**: 32 KB.
4. **Operating Voltage**: 3.3V (with 5V tolerant I/O).
5. **Input/Output Pins**:
   - **14 digital pins**, 12 of which can be used as PWM outputs.
   - **8 analog inputs**.
   - **SPI, I2C, UART** pins are available.
6. **IMU (Inertial Measurement Unit)**: LSM6DS3, a 6-axis sensor (accelerometer and gyroscope) for motion and rotation tracking.
7. **Compact size**: Follows the Nano form factor, making it ideal for projects with limited space.

### Compatibility with the Arduino Nano (ATmega328):
The **Arduino Nano 33 IoT** has the same pin configuration as the **Arduino Nano based on the ATmega328**, making it a great option for replacing or expanding existing projects using the classic **Arduino Nano**. This means that, in many cases, you can simply swap the board without needing to modify the existing hardware layout. Additionally, it provides the advantage of wireless connectivity (Wi-Fi and Bluetooth), enabling you to expand the capabilities of your projects.

This pin compatibility makes the Nano 33 IoT a natural choice if you want to upgrade a project to include IoT connectivity, while keeping the simplicity of the design and enabling easy integration of new features without significant board reconfiguration.






## References

* [Arduino® Nano 33 IoT](https://docs.arduino.cc/resources/datasheets/ABX00027-datasheet.pdf)
* [Connect the Arduino Nano 33 IoT with Ubidots over HTTP](https://help.ubidots.com/en/articles/3383755-connect-the-arduino-nano-33-iot-with-ubidots-over-http)