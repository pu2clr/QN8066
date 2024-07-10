/**
 * @mainpage QN8066 Arduino Library implementation
 *
 * Contact: pu2clr@gmail.com
 *
 * @details QN8066
 *
 * @author PU2CLR - Ricardo Lima Caratti
 * @date  2024-06
 * @copyright MIT Free Software model. See [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/QN8066/#mit-license).
 */

#include <QN8066.h>

/**
 * @brief   Checks communication with QN8066 via I2C
 * @details Checks if the QN8066 is available on the I2C bus. Remember that the QN8066 responds to the address 0x21
 * @return  true or false  
 */
bool QN8066::detectDevice() {

    Wire.begin();
    // check 0x21 I2C address
    Wire.beginTransmission(QN8066_I2C_ADDRESS);
    return  !Wire.endTransmission();
}

 /**
  * @brief  Scans the I2C bus and returns the addresses of the devices found.
  * @details Searches for devices connected to the I2C bus. The addresses of the devices found are stored in the "device" array.
  * @param device array of device addresses found.  
  * @return uint8_t number of devices found or 0 if no device found or error. 
  */
uint8_t QN8066::scanI2CBus(uint8_t *device) {

  uint8_t error, address;
  uint8_t idxDevice = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
       device[idxDevice] = address;
       idxDevice++;
    } else if (error == 4) {
       return 0; 
    }
  }
  return idxDevice;
} 

uint8_t QN8066::getRegister(uint8_t registerNumber) {

    Wire.beginTransmission(QN8066_I2C_ADDRESS);
    Wire.write(registerNumber);
    Wire.endTransmission();
    delayMicroseconds(QN8066_DELAY_COMMAND);

    Wire.requestFrom(QN8066_I2C_ADDRESS, 1);
    return Wire.read();

}

void  QN8066::setRegister(uint8_t registerNumber, uint8_t value) {

    Wire.beginTransmission(QN8066_I2C_ADDRESS);
    Wire.write(registerNumber);
    Wire.write(value);
    Wire.endTransmission();
    delayMicroseconds(QN8066_DELAY_COMMAND);

} 

