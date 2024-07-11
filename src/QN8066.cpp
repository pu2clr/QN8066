/**
 * @mainpage QN8066 Arduino Library implementation
 *
 * Contact: pu2clr@gmail.com
 *
 * @details QN8066
 *
 * @author PU2CLR - Ricardo Lima Caratti
 * @date  2024-06
 * @copyright MIT Free Software model. See [Copyright (c) 2019 Ricardo Lima
 * Caratti](https://pu2clr.github.io/QN8066/#mit-license).
 */

#include <QN8066.h>

/**
 * @brief   Checks communication with QN8066 via I2C
 * @details Checks if the QN8066 is available on the I2C bus. Remember that the
 * QN8066 responds to the address 0x21
 * @return  true or false
 */
bool QN8066::detectDevice() {

  Wire.begin();
  // check 0x21 I2C address
  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  return !Wire.endTransmission();
}

/**
 * @brief  Scans the I2C bus and returns the addresses of the devices found.
 * @details Searches for devices connected to the I2C bus. The addresses of the
 * devices found are stored in the "device" array.
 * @param device array of device addresses found.
 * @return uint8_t number of devices found or 0 if no device found or error.
 */
uint8_t QN8066::scanI2CBus(uint8_t *device) {

  uint8_t error, address;
  uint8_t idxDevice = 0;

  Wire.begin();

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

void QN8066::setup() {
  delay(600);       // Chip power-up time 
  Wire.begin();

}

uint8_t QN8066::getRegister(uint8_t registerNumber) {

  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  Wire.write(registerNumber);
  Wire.endTransmission();
  delayMicroseconds(QN8066_DELAY_COMMAND);

  Wire.requestFrom(QN8066_I2C_ADDRESS, 1);
  return Wire.read();
}

void QN8066::setRegister(uint8_t registerNumber, uint8_t value) {

  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  Wire.write(registerNumber);
  Wire.write(value);
  Wire.endTransmission();
  delayMicroseconds(QN8066_DELAY_COMMAND);
}

  qn8066_status1 QN8066::getStatus1() {
    qn8066_status1 value;
    value.raw = this->getRegister(QN_STATUS1); 
    return value;
  }

  qn8066_status2 QN8066::getStatus2() {
    qn8066_status2 value;
    value.raw = this->getRegister(QN_STATUS2); 
    return value;
  }
  
  qn8066_status3 QN8066::getStatus3() {
    qn8066_status3 value;
    value.raw = this->getRegister(QN_STATUS3); 
    return value;

  }

 qn8066_system1  QN8066::getSystem1() {
    qn8066_system1 value;
    value.raw = this->getRegister(QN_SYSTEM1); 
    return value;
 }

 qn8066_system2  QN8066::getSystem2() {
    qn8066_system2 value;
    value.raw = this->getRegister(QN_SYSTEM2); 
    return value;
 }


/**
 * @brief sets the devive to RX
 */
void QN8066::setRX() {
  uint8_t value = 0B11100011;
  this->setRegister(QN_SYSTEM1, value);
}

/**
 * @brief sets the devive to TX
 */
void QN8066::setTX(float frequency) {
  this->setRegister(QN_SYSTEM1, 0B11100011);  // RESET the SYSTEM
  delay(200);
  // Setup the reference clock source, Image Rejection and the threshold to check valid channel
  this->setRegister(QN_XTAL_DIV0, 0B00010000); 
  this->setRegister(QN_XTAL_DIV1, 0b11101000); 

  // Set frequency 
  int16_t auxFreq = (int16_t) (( frequency - 60) / 0.05 ); 
  this->setRegister(QN_INT_CTRL, 0B00100000 | auxFreq>>8);
  this->setRegister(QN_TXCH, 0B11111111 & auxFreq);

  // Exit standby, enter TX
  this->setRegister(QN_SYSTEM1, 0b00001011);
  delay(200);
}



/**
 * @brief convert a given frequency to a channel
 * @details By programming channel index RXCH[9:0] or TXCH[9:0], the RF channel can be set to any frequency between 60 MHz ~ 108 MHz in 50 kHz steps. 
 * @details The channel index and RF frequency have the following relationship: FRF = (60 + 0.05 x Channel Index), where FRF is the RF frequency in MHz.
 * @param frequency 
 */
void QN8066::setChannel(float frequency) {
  uint16_t channel = (uint16_t) (round((frequency - 64) / 0.05f)) ;

  qn8066_int_ctrl value;
  value.raw = this->getRegister(QN_INT_CTRL);
  value.arg.TXCH = (channel >> 8);
  this->setRegister(QN_INT_CTRL, value.raw);
  this->setRegister(QN_TXCH, (uint8_t) (channel & 0xFF) );
  delay(100); // Channel switching - From any channel to any channel.
}

/**
 * @brief PA output power target control.
 * @details PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are 24-56.
 * @param pacValue 
 */
void QN8066::setPAC(uint8_t PA_TRGT) {

  //Reset aud_pk
 this->setRegister(QN_PAC, 0b10000000 | PA_TRGT );
 this->setRegister(QN_PAC, 0b00000000 | PA_TRGT );

}
 


