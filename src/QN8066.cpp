/**
 * @mainpage QN8066 Arduino Library implementation
  *
 * @details This is an Arduino library for the QN8066 FM RX/TX device (Digital FM Transceiver for Portable Devices).
 * @details The communication used by this library is I2C. 
 * @details You can see a complete documentation on <https://github.com/pu2clr/QN8066>
 * @details This library can be freely distributed. This means that you can copy, modify, merge, and redistribute the code for free under the terms of the [MIT License](https://github.com/pu2clr/QN8066/blob/main/LICENSE).   
 * @details There are examples that can help you in your project on <https://github.com/pu2clr/QN8066/tree/master/examples>
 * @see [General Documentation](https://pu2clr.github.io/QN8066/)
 * @author PU2CLR - Ricardo Lima Caratti -  pu2clr@gmail.com
 * @date  2024-06
 * @copyright MIT Free Software model. See [Copyright (c) 2024 Ricardo Lima Caratti](https://github.com/pu2clr/QN8066/blob/main/LICENSE).
 */

#include <QN8066.h>

/** @defgroup group01 Device Checking*/

/**
 * @ingroup group01 Detect Device
 * @brief   Checks communication with QN8066 via I2C
 * @details Checks if the QN8066 is available on the I2C bus. Remember that the QN8066 responds to the address 0x21
 * @return  true or false
 */
bool QN8066::detectDevice() {

  Wire.begin();
  // check 0x21 I2C address
  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  return !Wire.endTransmission();
}

/**
 * @ingroup group01 Scan I2C Devices
 * @brief  Scans the I2C bus and returns the addresses of the devices found.
 * @details Searches for devices connected to the I2C bus. The addresses of the devices found are stored in the "device" array.
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

/** @defgroup group02 Basic Functions*/

/**
 * @ingroup group02 I2C
 * @brief Gets register information
 *
 * @param registerNumber
 * @return uint8_t Value of the register
 */
uint8_t QN8066::getRegister(uint8_t registerNumber) {

  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  Wire.write(registerNumber);
  Wire.endTransmission();
  delayMicroseconds(QN8066_DELAY_COMMAND);

  Wire.requestFrom(QN8066_I2C_ADDRESS, 1);
  return Wire.read();
}

/**
 * @ingroup group02 I2C
 * @brief Stores a velue to a given register
 *
 * @param registerNumber
 * @param value
 */
void QN8066::setRegister(uint8_t registerNumber, uint8_t value) {

  Wire.beginTransmission(QN8066_I2C_ADDRESS);
  Wire.write(registerNumber);
  Wire.write(value);
  Wire.endTransmission();
  delayMicroseconds(QN8066_DELAY_COMMAND);
}

/**
 * @ingroup group02 Device Status
 * @brief Gets the current device Status stored in STATUS1 register
 * @return qn8066_status1
 */
qn8066_status1 QN8066::getStatus1() {
  qn8066_status1 value;
  value.raw = this->getRegister(QN_STATUS1);
  return value;
}

/**
 * @ingroup group02 Device Status
 * @brief Gets the current device Status stored in STATUS2 register
 * @return qn8066_status2
 */
qn8066_status2 QN8066::getStatus2() {
  qn8066_status2 value;
  value.raw = this->getRegister(QN_STATUS2);
  return value;
}

/**
 * @ingroup group02 Device Status
 * @brief Gets the current device Status stored in STATUS3 register
 * @return qn8066_status3
 */
qn8066_status3 QN8066::getStatus3() {
  qn8066_status3 value;
  value.raw = this->getRegister(QN_STATUS3);
  return value;
}

/**
 * @ingroup group02 Init Device
 * @brief Device initial configuration
 */

void QN8066::setup() {
  delay(600); // Chip power-up time
  Wire.begin();
}

/** @defgroup group03 RX Functions*/

/**
 * @ingroup group03 RX
 * @brief sets the devive to RX
 */
void QN8066::setRX() {
  uint8_t value = 0B11100011;
  this->setRegister(QN_SYSTEM1, value);
  // TODO...
}

/** @defgroup group04 TX Functions*/

/**
 * @ingroup group04 Start TX
 * @brief Sets the TX mode
 * @param frequency - Frequency to be set
 */
void QN8066::setTX(float frequency) {
  this->setRegister(QN_SYSTEM1, 0B11100011); // RESET the SYSTEM
  delay(200);

  // Setup the reference clock source, Image Rejection and the threshold to
  // check valid channel
  this->setRegister(QN_CCA, 0B00010000);

  // Sets the crystal oscillator divider
  this->setRegister(QN_XTAL_DIV0,
                    this->xtal_div & 0xFF); // Lower 8 bits of xtal_div[10:0].
  this->setRegister(QN_XTAL_DIV1,
                    (this->xtal_div >> 8) |
                        0B0001000); // Higher 3 bits of xtal_div[10:0].

  // Set frequency
  int16_t auxFreq = (int16_t)((frequency - 60) / 0.05);
  this->setRegister(QN_INT_CTRL, 0B00100000 | auxFreq >> 8);
  this->setRegister(QN_TXCH, 0B11111111 & auxFreq);

  // Exit standby, enter TX
  this->setRegister(QN_SYSTEM1, 0b00001011);
  delay(200);
}


/**
 * @ingroup group04  TX Setup
 * @brief Set TX Stereo or Mono
 * @details  
 * @param value (true = stereo; false = mono)
 */
void QN8066::setTxStereo( bool value ) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tx_mono = !value;
  this->setRegister(QN_SYSTEM2, system2.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief   Pre-emphasis and de-emphasis time constant
 * @details The valid values are 50 and 75. Any value not equal to 75 sets the Pre-emphasis to 50. 
 * @param value (valids values:  50 or 75);
 */
void QN8066::setTxPreEmphasis( uint8_t value ) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tc = (value == 75);
  this->setRegister(QN_SYSTEM2, system2.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief Gain of TX pilot to adjust pilot frequency deviation.
 * @details Refers to peak frequency deviation of MPX signal when audio input is full scale.  Valid values: between 7 and 10.
 * @details the frequency deviation is value (%) *  75 kHz.
 * @param value
 */
void QN8066::setTxPilotGain(uint8_t value) {
  qn8066_gplt gptl;

  if (value > 6 && value < 11) {
    gptl.raw = this->getRegister(QN_GPLT);
    gptl.arg.GAIN_TXPLT = value;
    this->setRegister(QN_GPLT, gptl.raw);
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief Sets volume control gain of analog portion
 * @details Valid values are 0 to 7.
 *
 * @param value
 */
void QN8066::setAudioAnalogGain(uint8_t value) {
  qn8066_vol_ctl vol_ctl;

  if (value < 8) {
    vol_ctl.raw = this->getRegister(QN_VOL_CTL);
    vol_ctl.arg.GAIN_ANA = value;
    this->setRegister(QN_VOL_CTL, vol_ctl.raw);
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief Sets set digital volume gain
 * @details Valid values are 0 to 5.
 *
 * @param value
 */
void QN8066::setAudioDigitalGain(uint8_t value) {
  qn8066_vol_ctl vol_ctl;

  if (value < 6) {
    vol_ctl.raw = this->getRegister(QN_VOL_CTL);
    vol_ctl.arg.GAIN_DIG = value;
    this->setRegister(QN_VOL_CTL, vol_ctl.raw);
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief DAC output control
 * @details If value is 0 (false), then "Normal operation"; if true, Hold DAC output to a fixed voltage.
 *
 * @param value  (true or false)
 */
void QN8066::setAudioDacHold(bool value) {
  qn8066_vol_ctl vol_ctl;

  vol_ctl.raw = this->getRegister(QN_VOL_CTL);
  vol_ctl.arg.DAC_HOLD = value;
  this->setRegister(QN_VOL_CTL, vol_ctl.raw);
}

/**
 * @ingroup group04  TX Setup
 * @brief Tx audio input mode selection
 * @details If value is 0 (false), then "Single ended"; if true, Differential
 *
 * @param value  (true or false)
 */
void QN8066::setAudioTxDiff(bool value) {
  qn8066_vol_ctl vol_ctl;

  vol_ctl.raw = this->getRegister(QN_VOL_CTL);
  vol_ctl.arg.TX_DIFF = value;
  this->setRegister(QN_VOL_CTL, vol_ctl.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief TX mode input impedance for both L/R channels
 * @details Sets the imput impedance  Left and Rihgt channels
 * 
 * | RIN Value |  Input impedance (k) | 
 * | --------- | -------------------- |
 * |  00  (0)  |          10          | 
 * |  01  (1)  |          20          |
 * |  10  (2)  |          40          | 
 * |  11  (3)  |          80          | 
 *
 * @param value  (true or false)
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 35
 */
void QN8066::setTxInputImpedance(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.RIN = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief TX digital gain
 * @details TX digital gain
 * 
 * | TXAGC_GDB |  Digital gain        | 
 * | --------- | -------------------- |
 * |  00  (0)  |          0 dB        | 
 * |  01  (1)  |          1 dB        |
 * |  10  (2)  |          2 dB        | 
 * |  11  (3)  |         Reserved     | 
 *
 * @param value  
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 35
 */
void QN8066::setTxDigitalGain(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.TXAGC_GDB = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief TX input buffer gain
 * @details Sets TX input buffer gain
 * | TXAGC_GVGA  | Attenuation depending on RIN - 0, 1, 2 AND 3 RESPECTIVELY  |
 * | ----------  | ---------------------------------------------------------- |
 * |  0 - 000    |  3; -3; -9; -15   |
 * |  1 - 001    |  6;  0; -6; -12   |
 * |  2 - 010    |  9;  3; -3; -9    |
 * |  3 - 011    | 12;  6;  0; -6    |
 * |  4 - 100    | 15;  9;  3; -3    |
 * |  5 - 101    | 18; 12;  6;  0    |
 * |  Others     | Reserved |
 * @code {.c++}
 *  QN8066 tx; 
 *  ...
 *  tx.setsetTxInputImpedance(0);  // Sets the input impedance to 10 Kohms
 *  tx.setTxInputBufferGain(5);    // Now the Gain will be 18dB
 *  ... 
 * @endcode
 * @param value  
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 35
 */
void QN8066::setTxInputBufferGain(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.TXAGC_GVGA = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
}


/**
 * @ingroup group04  TX Setup
 * @brief TX soft clipping enable
 * @details  
 * @param value  (true = enabled; false = disabled)
 */
void QN8066::setTxSoftClipping( bool value) {
  qn8066_reg_vga reg_vga; 
  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.tx_sftclpen = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);  
}


/**
 * @ingroup group04  TX Setup
 * @brief Specify total TX frequency deviation.
 * @details TX frequency deviation = 0.69KHz*TX_FEDV. The default value is 108 (Binary: 01101100)
 * @param value  
 */
 void QN8066::setTxFrequencyDerivation(uint8_t value) {
  this->setRegister(QN_FDEV, value );
 }

/**
 * @ingroup group04 TX Channel
 * @brief convert a given frequency to a channel
 * @details By programming channel index RXCH[9:0] or TXCH[9:0], the RF channel
 * can be set to any frequency between 60 MHz ~ 108 MHz in 50 kHz steps.
 * @details The channel index and RF frequency have the following relationship:
 * FRF = (60 + 0.05 x Channel Index), where FRF is the RF frequency in MHz.
 * @param frequency
 */
void QN8066::setTxChannel(float frequency) {
  uint16_t channel = (uint16_t)((frequency - 60) / 0.05);

  qn8066_int_ctrl value;
  value.raw = this->getRegister(QN_INT_CTRL);
  value.arg.TXCH = (channel >> 8);
  this->setRegister(QN_INT_CTRL, value.raw);
  this->setRegister(QN_TXCH, (uint8_t)(channel & 0xFF));
  delay(100);
}

/**
 * @ingroup group03 PA Control
 * @brief PA output power target control.
 * @details PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values are
 * 24-56.
 * @param pacValue
 */
void QN8066::setPAC(uint8_t PA_TRGT) {

  // Reset aud_pk
  this->setRegister(QN_PAC, 0b10000000 | PA_TRGT);
  this->setRegister(QN_PAC, 0b00000000 | PA_TRGT);
}



/** @defgroup group05 TX RDS Setup*/

/**
 * @ingroup group05 TX RDS
 * @brief Transmitter RDS enable
 * @details Enable RDS service 
 * @param value (true = enabled; false = disabled)
 * @see  Pages 20 and 21 of the Datasheet (Register SYSTEM2)
 */
void QN8066::setTxRDS(bool value) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tx_rdsen = value;
  this->setRegister(QN_SYSTEM2, system2.raw);
}

/**
 * @ingroup group05 TX RDS
 * @brief RDS transmitting ready.
 * @details If user want the chip transmitting all the 8 bytes in RDS0~RDS7, user should toggle this bit. 
 * @details description the chip internally will fetch these bytes after completing transmitting of current group.
 * @see  Pages 20 and 21 of the Datasheet (Register SYSTEM2)
 */
void QN8066::setTxToggleRDSReady() {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.rdsrdy = !system2.arg.rdsrdy;
  this->setRegister(QN_SYSTEM2, system2.raw);
}

/**
 * @ingroup group05 TX RDS
 * @brief RDS TX Updated
 * @details To transmit the 8 bytes in RDS0~RDS7, user should toggle the register bit RDSRDY (See SYSTEM2 register).
 * @details Then the chip internally fetches these bytes after completing transmitting of current group.
 * @details Once the chip internally fetched these bytes, it will toggle this bit, and user can write in another group.
 * @return true 
 * @return false 
 */
bool QN8066::getTxRDSUpdated() { 
    return this->getStatus3().arg.RDS_TXUPD;
}

/**
 * @ingroup group05 TX RDS
 * @brief Writes the RDS data bytes to be sent (SEE TX_RDSD0 to TX_RDSD7 registers)
 * @details The data written into RDSD0~RDSD7 cannot be sent out if user didn’t toggle RDSTXRDY to allow the data loaded into internal transmitting buffer.
 * @param text (point to array of char with 8 bytes to be loaded into the RDS data buffer)
 */
void QN8066::writeTxRDSBuffer(char *text) { 
  for (uint8_t address = QN_TX_RDSD0; address <= QN_TX_RDSD7; address++ ) {
    this->setRegister(address, *text++);
  }
}

/**
 * @ingroup group05 TX RDS
 * @brief Sets RDS frequency deviation.
 * @details RDS frequency deviation = 0.35KHz*RDSFDEV in normal mode. 
 * @details RDS frequency deviation = 0.207KHz*RDSFDEV in 4k mode and private mode.
 * @param freq ( valid values: from 0 to 127)
 * @see Datasheet, register RDS (0x26), page 34. 
 */
void QN8066::setRDSFrequencyDeviation(uint8_t freq) {
  qn8066_rds rds;
  rds.raw = this->getRegister(QN_RDS);
  rds.arg.RDSFDEV = freq;
  this->setRegister(QN_RDS, rds.raw);  
} 

/**
 * @ingroup group05 TX RDS
 * @brief Audio Line-in enable control
 * @param value (true = enabled; false = disabled)
 * @see Datasheet, register RDS (0x26), page 34. 
 */
void QN8066::setRDSLineIn(bool value) {
  qn8066_rds rds;
  rds.raw = this->getRegister(QN_RDS);
  rds.arg.line_in_en = value;
  this->setRegister(QN_RDS, rds.raw);  
} 