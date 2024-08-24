/**
 * @mainpage QN8066 Arduino Library implementation
  *
 * @details This is an Arduino library for the QN8066 FM RX/TX device (Digital FM Transceiver for Portable Devices).
 * @details The communication used by this library is I2C. 
 * @details You can see a complete documentation on <https://github.com/pu2clr/QN8066> or <https://pu2clr.github.io/QN8066/>
 * @details 
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
 * @details 8066 commands. It provides a means to run commands that are not currently supported by the standard API.
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
 * @details 8066 commands. It provides a means to run commands that are not currently supported by the standard API.
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
 * @see QUINTIC Datasheet - Rev 0.2,  Page 24. 
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   Serial.begin(9600);
 *   tx.setup();
 *   Serial.print("\nStarting the system...");
 *   tx.setTX(1069); // Sets the transmitter to 106.9 MHz 
 *   tx.setTxStereo(true);
 *   qn8066_status1 s1 = tx.getStatus1();
 *   Serial.println(s1.arg.FSM); // Shows  FSM state code - QUINTIC Datasheet - Rev 0.2,  Page 24. 
 * }
 * void loop() {
 *   delay(5);
 *}
 * @endcode
 *   
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
 * @see QUINTIC Datasheet - Rev 0.2,  Page 28.  
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   Serial.begin(9600);
 *   tx.setup();
 *   Serial.print("\nStarting the system...");
 *   tx.setTX(1069); // Sets the transmitter to 106.9 MHz 
 *   tx.setTxStereo(true);
 *   qn8066_status2 s2 = tx.getStatus2();
 *   Serial.println(s2.arg.RDSSYNC); // Shows RDS block synchronous indicator. 
 * }
 * void loop() {
 *   delay(5);
 *}
 * @endcode 
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
 * @see QUINTIC Datasheet - Rev 0.2,  Page 30.   
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   Serial.begin(9600);
 *   tx.setup();
 *   Serial.print("\nStarting the system...");
 *   tx.setTX(1069); // Sets the transmitter to 106.9 MHz 
 *   tx.setTxStereo(true);
 *   ...
 *   qn8066_status3 s3 = tx.getStatus2();
 *   Serial.println(s3.arg.RDS_TXUPD); // Checks RDS TX update 
 * }
 * void loop() {
 *   delay(5);
 *}
 * @endcode 
 */
qn8066_status3 QN8066::getStatus3() {
  qn8066_status3 value;
  value.raw = this->getRegister(QN_STATUS3);
  return value;
}

/**
 * @ingroup group02 Init Device
 * @brief Device initial configuration
 * @details The functions described in this group are used to configure the QN8066.
 */

/**
 * @ingroup group02 Init Device
 * @brief Starts the  QN8066 instance.  
 * @details It is a minimalist code to initiate the QN8066 device. Use the function setup if you want more option during the start process.
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   // Call setup setting Divider = 1000, Stereo, RDS on and PreEmphasis (tc) 75us
 *   tx.begin(); 
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */

void  QN8066::begin() {
  delay(600); // Chip power-up time

  this->system1.raw = 0B11100011;
  this->system2.raw = 0;
  this->cca.raw = this->getRegister(QN_CCA);
  this->cca.arg.xtal_inj = 1; 
  this->gplt.raw = this->getRegister(QN_GPLT);
  this->fdev.raw = this->getRegister(QN_FDEV);
  this->rds.raw = this->getRegister(QN_RDS);
  this->reg_vga.raw = this->getRegister(QN_REG_VGA);
  this->int_ctrl.raw = this->getRegister(QN_INT_CTRL);
  this->pac.raw = this->getRegister(QN_PAC);
  this->vol_ctl.raw = this->getRegister(QN_VOL_CTL);

  Wire.begin();
}

/**
 * @ingroup group02 Init Device
 * @brief Set transmission request
 * @details This function enable or disable transmission
 * @param value - 0 = disable; 1 = enable
 */
inline void QN8066::setTxMode(uint8_t value) {
  this->system1.arg.txreq = value;
  this->setRegister(QN_SYSTEM1, this->system1.raw);
}

/**
 * @ingroup group02 Init Device
 * @brief Stops transmitting
 */
void QN8066::stopTransmitting() {
  this->setTxMode(0);
}

/**
 * @ingroup group02 Init Device
 * @brief Starts transmitting
 */
void QN8066::startTransmitting() {
  this->setTxMode(1);
}

/**
 * @ingroup group02 Init Device
 * @brief QN8066 initial configuration
 * @details This function can be called without arguments (parameters). In this case, the default value will be assumed. See the following list of parameters.
 * @param xtalDiv  - Divider based on frequency select of reference clock source. Default 1000 (see Datasheet pag. 18 and 23).
 * @param mono - If false, the TX will start stereo mode. Default false.
 * @param rds  - if true, TX will start with RDS on. Default false.
 * @param PreEmphasis - Valid values: 0 or 1 (0=50us; 1=75us). Default 0.
 * @param xtalInj - type of the reference clock source (0 = Inject sine-wave clock; 1 = Inject digital clock).
 * @param imageRejection - Image Rejection (0=LO<RF, image is in lower side; 1=LO>RF, image is in upper side).
 * @param txSoftClipThreshold - TX soft clip threshold. Default 0. See Datasheet page 34.
 * @param oneMinutOff - Selection of 1 minute time for PA off when no audio (3 = Infinity (never); 2=59s; 1=58s; 0=57s).
 * @param gainTxPLT - Gain of TX pilot to adjust pilot frequency deviation. See Datasheet page 34.
 * @param txFreqDev - Specify total TX frequency deviation. TX frequency deviation = 0.69KHz*TX_FEDV. Default 125.
 * @param rdsLineIn - Audio Line-in enable control. Default 0  
 * @param rdsFreqDev - RDS frequency deviation. RDS frequency deviation = 0.35KHz*RDSFDEV in normal mode. Default 60
 * @param inImpedance - TX mode input impedance for both L/R channels (Default 1 => 20K )
 * @param txAgcDig - TX digital gain (Default 0)
 * @param txAgcBuffer - TX input buffer gain (Default 1)
 * @param txSoftClip - TX soft clipping enable (Default 0 - Disable)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   // Call setup setting Divider = 1000, Stereo, RDS on and PreEmphasis (tc) 75us
 *   tx.setup(1000, false, true, 1); 
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
void QN8066::setup(uint16_t xtalDiv,  
                   bool mono, bool rds, 
                   uint8_t PreEmphasis,  uint8_t xtalInj, uint8_t imageRejection,
                   uint8_t txSoftClipThreshold,  uint8_t oneMinutOff, uint8_t gainTxPLT,
                   uint8_t txFreqDev,  uint8_t rdsLineIn, uint8_t rdsFreqDev, 
                   uint8_t inImpedance, uint8_t txAgcDig, uint8_t txAgcBuffer, uint8_t txSoftClip ) {
  
  Wire.begin();

  delay(200); // Chip power-up time

  this->xtal_div = xtalDiv;

  this->system1.raw = 0B11100011;
  
  this->system2.raw = 0;
  this->system2.arg.tx_mono = mono;   // Default stereo
  this->system2.arg.tx_rdsen = rds;   // RDS ON
  this->system2.arg.tc = PreEmphasis; // Default 50

  this->cca.raw = this->getRegister(QN_CCA);
  this->cca.arg.xtal_inj = xtalInj;
  this->cca.arg.imr = imageRejection;
  this->cca.arg.SNR_CCA_TH = 0B010000;

  this->gplt.raw = this->getRegister(QN_GPLT);
  this->gplt.arg.GAIN_TXPLT = gainTxPLT;
  this->gplt.arg.t1m_sel = oneMinutOff;
  this->gplt.arg.tx_sftclpth = txSoftClipThreshold; 

  // FDEV register 
  this->fdev.raw = txFreqDev; 

  // RDS register
  this->rds.arg.line_in_en = rdsLineIn;
  this->rds.arg.RDSFDEV = rdsFreqDev;

  // this->int_ctrl.raw = this->getRegister(QN_INT_CTRL); 

  // REG_VGA
  // this->reg_vga.raw = this->getRegister(QN_REG_VGA);
  this->reg_vga.arg.RIN = inImpedance;
  this->reg_vga.arg.TXAGC_GDB = txAgcDig; 
  this->reg_vga.arg.TXAGC_GVGA = txAgcBuffer;
  this->reg_vga.arg.tx_sftclpen = txSoftClip;

}

/** 
 * @defgroup group03 RX Functions 
 * @brief QN8066 Receiver funtions
 * @details The functions defined in this group are responsible for controlling the reception of the QN8066.    
*/

/**
 * @ingroup group03 RX
 * @brief sets the devive to RX
 * @param frequency - frequency that the receiver will start
 */
void QN8066::setRX(uint16_t frequency) {
  this->system1.raw = 0B11100011;
  this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 11100011  =>  swrst = 1; recal = 1; stnby = 1; ccs_ch_dis = 1; cca_ch_dis = 1
  this->setRegister(QN_SYSTEM2, this->system2.raw); 
  this->system2.arg.rdsrdy = !(this->system2.arg.rdsrdy); // Toggle 
  this->setRegister(QN_SYSTEM2, this->system2.raw); 
  this->setRegister(QN_CCA, this->cca.raw); // CCA => 01010000 => xtal_inj = 0; imr = 1; SNR_CCA_TH = 010000
  // Sets the crystal oscillator divider
  this->setRegister(QN_XTAL_DIV0, this->xtal_div & 0xFF); // Lower 8 bits of xtal_div[10:0].
  this->setRegister(QN_XTAL_DIV1, (this->xtal_div >> 8) |  0B0001000); // Higher 3 bits of xtal_div[10:0].
  this->setRegister(QN_XTAL_DIV2, 0B01011100); // XTAL_DIV2 = > 01011100 (It is the default value)

  // this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 00001011 => Set TX On
  this->setRegister(QN_FDEV, this->fdev.raw);    // FDEV => 01111101 => 125 (Decimal)
  this->setRegister(QN_RDS, this->rds.raw);     // RDS => 00111100 => Line_in_en = 0; RDSFDEV = 60 (Decimal) 
  this->setRegister(QN_GPLT, this->gplt.raw);    // GPLT => 00111001 => Tx_sftclpth = 00 (12’d2051 - 3db back off from 0.5v); t1m_sel = 11 (Infinity); GAIN_TXPLT = 1001 (9% 75 kHz)

  int16_t auxFreq = (frequency - 600)  * 2;
  qn8066_rx_ch rxch; 
  qn8066_ch_step ch_step;
  ch_step.raw = this->getRegister(QN_CH_STEP);
  ch_step.arg.RXCH  =  0B0000000000000011 & (auxFreq >> 8);
  rxch.RXCH =  0B0000000011111111 & auxFreq;
  this->setRegister(QN_CH_STEP, ch_step.raw );
  this->setRegister(QN_RX_CH,rxch.RXCH);

  // Checking unkown registers
  // this->setRegister(0x49, 0B11101000); 
  this->setRegister(0x49, 0B11011111); 
  this->setRegister(0x6E, 0B11111111); 

  this->system1.raw = 0B00010011; // Receiver request
  this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 00001011 => txreq = 1; ccs_ch_dis = 1; cca_ch_dis = 1 
  // this->setRegister(QN_REG_VGA, 0B01011011); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)
  this->setRegister(QN_REG_VGA, this->reg_vga.raw); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)
  delay(100);
}

/**
 * @ingroup group03 RX
 * @brief sets the receiver frequency
 */
void QN8066::setRxFrequency(uint16_t frequency) {
  int16_t auxFreq = (frequency - 600)  * 2;
  qn8066_rx_ch rxch; 
  qn8066_ch_step ch_step;
  ch_step.raw = this->getRegister(QN_CH_STEP);
  ch_step.arg.RXCH  =  0B0000000000000011 & (auxFreq >> 8);
  rxch.RXCH =  0B0000000011111111 & auxFreq;
  this->setRegister(QN_CH_STEP, ch_step.raw );
  this->setRegister(QN_RX_CH,rxch.RXCH);

}


/**
 * @ingroup group03 RX
 * @brief Enables RDS for RX
 */
void QN8066::rdsEnableRX(bool value) {
  this->system2.arg.rx_rdsen = value; 
  this->setRegister(QN_SYSTEM2, this->system2.raw);
}


/**
 * @ingroup group03 RX
 * @brief Mute or unmute the audio in receiver mode
 * @param value - if true mutes the audio; if false, mute disabled 
 */
void QN8066::setAudioMuteRX(bool value) {
  this->system2.arg.rx_mute = value; 
  this->setRegister(QN_SYSTEM2, this->system2.raw);
}

/**
 * @ingroup group03 RX
 * @brief Gets the current SNR (Estimated RF input)
 * @return current SNR in dB
 */
uint8_t QN8066::getRxSNR() {
  return  this->getRegister(QN_SNR);
}


/**
 * @ingroup group03 RX
 * @brief Gets the current RSSI
 * @details In-band signal RSSI (Received signal strength indicator) dBuV value. dBuV=RSSI-49
 * @return current RSSI in dB
 */
uint8_t QN8066::getRxRSSI() {
    return  this->getRegister(QN_RSSISIG);
}

/**
 * @ingroup group03 RX
 * @brief   Gets a valid or no valid rx channel
 * @details RXCCA status flag. To indicate whether a valid channel is found during RX CCA. 
 * @details If a valid channel is found, channel index will stay there, and RXCCA_FAIL=0; 
 * @details otherwise, it will stay at the end of scan range and RXCCA_FAIL=1.
 * @return  true   valid channel
 * @return  false  not found a valid channel
 */
bool QN8066::isValidRxChannel() {
  qn8066_status1 s1;
  s1.raw = this->getRegister(QN_STATUS1);
  return !(s1.arg.rxcca_fail);
}

/**
 * @ingroup group03 RX
 * @brief   Receiver status 
 * @return  true - Is receiving 
 */
bool QN8066::isRxReceiving() {
  qn8066_status1 s1;
  s1.raw = this->getRegister(QN_STATUS1);
  return s1.arg.RXSTATUS;

}

/**
 * @ingroup group03 RX
 * @brief   RX AGC Status
 * @details RX AGC Settling status
 * @return  true - Settled / Stabled
 */
bool QN8066::isRxAgcStable() {
  qn8066_status1 s1;
  s1.raw = this->getRegister(QN_STATUS1);
  return s1.arg.RXAGCSET;
}

/**
 * @ingroup group03 RX
 * @brief   Stereo receiving status
 * @return  true - Stereo
 */
bool QN8066::isRxStereo() {
  qn8066_status1 s1;
  s1.raw = this->getRegister(QN_STATUS1);
  return !(s1.arg.ST_MO_RX);
}

/**
 * @ingroup group03 RX
 * @brief   Scans a station
 * @details Searches for a station within a specified frequency range.
 * @param startFrequency - initial frequency to start the search.
 * @param stopFrequyency - final frequency to stop the search.
 * @param frequencyStep  - 0 = 50KHz; 1 = 100KHz; 2 =  200KHz
 */
void QN8066::scanRxStation(uint16_t startFrequency, uint16_t stopFrequyency, uint8_t frequencyStep ) {

  qn8066_ch_start start; 
  qn8066_ch_stop stop; 
  qn8066_ch_step step; 

  step.raw = this->getRegister(QN_CH_STEP);

  int16_t auxFreq = (startFrequency - 600)  * 2;
  start.CH_START =  0B0000000011111111 & auxFreq;
  step.arg.CH_STA = auxFreq >> 8; 

  auxFreq = (stopFrequyency - 600)  * 2;
  stop.CH_STOP =  0B0000000011111111 & auxFreq;
  step.arg.CH_STP = auxFreq >> 8;

  step.arg.CH_FSTEP =  frequencyStep; 

  this->setRegister(QN_CH_START,start.raw);
  this->setRegister(QN_CH_STOP, stop.raw);
  this->setRegister(QN_CH_STEP, step.raw);
} 

/** 
 * @defgroup group04 TX Functions
 * @details The functions defined in this group are responsible for controlling the transmitting  of the QN8066.
 */

/**
 * @ingroup group04 Start TX
 * @brief Sets the TX mode
 * @details To avoid working with the float data type, the frequency parameter must be the desired frequency multiplied by 10.
 * @details For example, if the user wants to tune to 106.9 MHz, the parameter to be sent is 1067.
 * @details This approach reduces the size of the final code (binary) as well as avoids the inaccuracies of floating-point mathematical operations.
 * @param frequency - Frequency to be set
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 * }
 *
 * void loop() {
 * }
 * @endcode 
 * @todo Under improvements 
 */
void QN8066::setTX(uint16_t frequency) {
  this->system1.raw = 0B11100011;
  this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 11100011  =>  swrst = 1; recal = 1; stnby = 1; ccs_ch_dis = 1; cca_ch_dis = 1
  this->setRegister(QN_SYSTEM2, this->system2.raw); 
  this->system2.arg.rdsrdy = !(this->system2.arg.rdsrdy); // Toggle 
  this->setRegister(QN_SYSTEM2, this->system2.raw); 
  this->setRegister(QN_CCA, this->cca.raw); // CCA => 01010000 => xtal_inj = 0; imr = 1; SNR_CCA_TH = 010000
  // Sets the crystal oscillator divider
  this->setRegister(QN_XTAL_DIV0, this->xtal_div & 0xFF); // Lower 8 bits of xtal_div[10:0].
  this->setRegister(QN_XTAL_DIV1, (this->xtal_div >> 8) |  0B0001000); // Higher 3 bits of xtal_div[10:0].
  this->setRegister(QN_XTAL_DIV2, 0B01011100); // XTAL_DIV2 = > 01011100 (It is the default value)

  // this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 00001011 => Set TX On
  this->setRegister(QN_FDEV, this->fdev.raw);    // FDEV => 01111101 => 125 (Decimal)
  this->setRegister(QN_RDS, this->rds.raw);     // RDS => 00111100 => Line_in_en = 0; RDSFDEV = 60 (Decimal) 
  this->setRegister(QN_GPLT, this->gplt.raw);    // GPLT => 00111001 => Tx_sftclpth = 00 (12’d2051 - 3db back off from 0.5v); t1m_sel = 11 (Infinity); GAIN_TXPLT = 1001 (9% 75 kHz)

  int16_t auxFreq = (frequency - 600)  * 2;
  this->int_ctrl.raw =  0B00100000 | auxFreq >> 8;
  this->setRegister(QN_INT_CTRL,this->int_ctrl.raw );
  this->setRegister(QN_TXCH, 0B11111111 & auxFreq);

  // Checking unkown registers
  // this->setRegister(0x49, 0B11101000); 
  this->setRegister(0x49, 0B11011111); 
  this->setRegister(0x6E, 0B11111111); 

  this->system1.raw = 0B00001011;
  this->setRegister(QN_SYSTEM1, this->system1.raw); // SYSTEM1 => 00001011 => txreq = 1; ccs_ch_dis = 1; cca_ch_dis = 1 
  // this->setRegister(QN_REG_VGA, 0B01011011); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)
  this->setRegister(QN_REG_VGA, this->reg_vga.raw); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)
  delay(100);
}


/**
 * @ingroup group02 Init Device
 * @brief QN8066 initial configuration of the of reference clock source
 * @details Reference clock source based on the active crystal or signal generator you are using. 
 * @details The table below shows some tested sources (active crystal or signal generator) and divider values 
 * |   Source (kHz)  | Divider       | 
 * | --------------  | ------------- |
 * |  32.768         |      1        | 
 * |  < 1,310.720    | did not work  |   
 * |  1,310.720      |    40         | 
 * |  1,638.400      |    50         | 
 * |  3,276.800      |   100         | 
 * |  13,107.200     |   400         | 
 * |  16,384.000     |   500         | 
 * |  32,768,000     |  1000         |  
 * @param xtalDiv  - Divider based on frequency select of reference clock source. Default 1000 (see Datasheet pag. 18 and 23).
 * @param xtalInj - type of the reference clock source (0 = Inject sine-wave clock; 1 = Inject digital clock).
 * @param imageRejection - Image Rejection (0=LO<RF, image is in lower side; 1=LO>RF, image is in upper side).
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.begin();
 *   tx.setXtal(1,1,1);
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 * }
 *
 * void loop() {
 * }
 * @endcode 
*/
void QN8066::setXtal(uint16_t divider, uint8_t xtalInj, uint8_t imageRejection) {
    this->xtal_div = divider;
    this->cca.arg.xtal_inj = xtalInj;
    this->cca.arg.imr = imageRejection;
}




/**
 * @ingroup group04  TX Setup
 * @brief Set TX Stereo or Mono
 * @details  
 * @param value (true = stereo; false = mono)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxStereo(true);
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
void  QN8066::setTxStereo( bool value ) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tx_mono = !value;
  this->setRegister(QN_SYSTEM2, system2.raw);
  this->system2 = system2;
}

/**
 * @ingroup group04  TX Setup
 * @brief Set TX Stereo or Mono (Same setTxStereo )
 * @details  
 * @param value (1 = Mono, 0 = Stereo )
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxMono(0); // Sets Stereo
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
void  QN8066::setTxMono(uint8_t value) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tx_mono = value;
  this->setRegister(QN_SYSTEM2, system2.raw);
  this->system2 = system2;  
} 

/**
 * @ingroup group04  TX Setup
 * @brief Gets TX Stereo or Mono setup
 * @return uint8_t -  1 = Mono; 0 = Stereo
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   if ( tx.getTxMono() ) {
 *      // It is in mono mode
 *   }
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
uint8_t QN8066::getTxMono() {
  return this->system2.arg.tx_mono;
} 


/**
 * @ingroup group04  TX Setup
 * @brief   Pre-emphasis and de-emphasis time constant
 * @details The valid values are 50 and 75. Any value not equal to 75 sets the Pre-emphasis to 50. 
 * @param value (valids values:  50 or 75);
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxPreEmphasis(75);
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
void QN8066::setTxPreEmphasis( uint8_t value ) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tc = (value == 75);
  this->setRegister(QN_SYSTEM2, system2.raw);
  this->system2 = system2;
}


/**
 * @ingroup group04  TX Setup
 * @brief   Pre-emphasis and de-emphasis time constant - Same setTxPreEmphasis.
 * @details The valid values are 0 and 1. 
 * @param value (valids values:  0 = 50us or 1 = 75us );
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setPreEmphasis(1); // 75 us
 * }
 *
 * void loop() {
 * }
 * @endcode 
 */
void QN8066::setPreEmphasis( uint8_t value ) {
  qn8066_system2 system2;
  system2.raw = this->getRegister(QN_SYSTEM2);
  system2.arg.tc = value;
  this->setRegister(QN_SYSTEM2, system2.raw);
  this->system2 = system2;
}

/**
 * @ingroup group04  TX Setup
 * @brief Gain of TX pilot to adjust pilot frequency deviation.
 * @details Refers to peak frequency deviation of MPX signal when audio input is full scale.  Valid values: between 7 and 10.
 * @details the frequency deviation is value (%) *  75 kHz.
 * @param value
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxPilotGain(7);   // 7 * 75 kHz
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setTxPilotGain(uint8_t value) {
  qn8066_gplt gptl;

  if (value > 6 && value < 11) {
    gptl.raw = this->getRegister(QN_GPLT);
    gptl.arg.GAIN_TXPLT = value;
    this->setRegister(QN_GPLT, gptl.raw);
    this->gplt = gptl;
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief TX soft clip threshold
 * @details See table below. 

 * | tx_sftclpth | value |
 * | ----------  | ----- |
 * |  0 - 00     | 12’d2051 (3db back off from 0.5v) |
 * |  1 - 01     | 12’d1725 (4.5db back off from 0.5v) |
 * |  2 - 10     | 12’d1452 (6db back off from 0.5v) |
 * |  3 - 11     | 12’d1028 (9db back off from 0.5v) |
 * @param value
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxSoftClipThreshold(2);   // 2 = 12’d1452 (6db back off from 0.5v) 
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setTxSoftClipThreshold(uint8_t value) {
  qn8066_gplt gptl;
  gptl.raw = this->getRegister(QN_GPLT);
  gptl.arg.tx_sftclpth = value;
  this->setRegister(QN_GPLT, gptl.raw);
  this->gplt = gptl;
}

/**
 * @ingroup group04  TX Setup
 * @brief Set of 1 minute time for PA off when no audio.
 * @param value (true = Infinity (never turn it off); false = turn it ofafter about 1 minue )
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxOffAfterOneMinuteNoAudio(false);   // Do not sleep after 1 minute with no audio
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setTxOffAfterOneMinuteNoAudio(bool value) {
  qn8066_gplt gptl;
  gptl.raw = this->getRegister(QN_GPLT);
  gptl.arg.t1m_sel = (value)? 2:3;
  this->setRegister(QN_GPLT, gptl.raw);
  this->gplt = gptl;
 } 

/**
 * @ingroup group04  TX Setup
 * @brief Sets PA Off after  1 minute time  when no audio.
 * @details do almost the same setTxOffAfterOneMinuteNoAudio
 * @param value (3 = Infinity (never turn it off); 2 = off after 59s; 1 = off after 58s; 0 = off after 57s  )
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxOffAfterOneMinute(3);   // Never sleep
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setTxOffAfterOneMinute(uint8_t value) {
  qn8066_gplt gptl;
  gptl.raw = this->getRegister(QN_GPLT);
  gptl.arg.t1m_sel = value;
  this->setRegister(QN_GPLT, gptl.raw);
  this->gplt = gptl;
 } 

/**
 * @ingroup group04  TX Setup
 * @brief Sets volume control gain of analog portion
 * @details Valid values are 0 to 7.
 * @param value
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setAudioAnalogGain(5);   // -24 dB
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setAudioAnalogGain(uint8_t value) {
  qn8066_vol_ctl vol_ctl;

  if (value < 8) {
    vol_ctl.raw = this->getRegister(QN_VOL_CTL);
    vol_ctl.arg.GAIN_ANA = value;
    this->setRegister(QN_VOL_CTL, vol_ctl.raw);
    this->vol_ctl = vol_ctl;
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief Sets set digital volume gain
 * @details Valid values are 0 to 5.
 * @param value
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setAudioDigitalGain(3);   // -3 dB
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::setAudioDigitalGain(uint8_t value) {
  qn8066_vol_ctl vol_ctl;

  if (value < 6) {
    vol_ctl.raw = this->getRegister(QN_VOL_CTL);
    vol_ctl.arg.GAIN_DIG = value;
    this->setRegister(QN_VOL_CTL, vol_ctl.raw);
    this->vol_ctl = vol_ctl;
  }
}

/**
 * @ingroup group04  TX Setup
 * @brief DAC output control
 * @details If value is 0 (false), then "Normal operation"; if true, Hold DAC output to a fixed voltage.
 * @param value  (true or false)
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setAudioDacHold(false);   // Normal operation
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::setAudioDacHold(bool value) {
  qn8066_vol_ctl vol_ctl;

  vol_ctl.raw = this->getRegister(QN_VOL_CTL);
  vol_ctl.arg.DAC_HOLD = value;
  this->setRegister(QN_VOL_CTL, vol_ctl.raw);
  this->vol_ctl = vol_ctl;
}

/**
 * @ingroup group04  TX Setup
 * @brief Tx audio input mode selection
 * @details If value is 0 (false), then "Single ended"; if true, Differential
 * @param value  (true or false)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setAudioTxDiff(true);   // Differential
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
void QN8066::setAudioTxDiff(bool value) {
  qn8066_vol_ctl vol_ctl;

  vol_ctl.raw = this->getRegister(QN_VOL_CTL);
  vol_ctl.arg.TX_DIFF = value;
  this->setRegister(QN_VOL_CTL, vol_ctl.raw);
  this->vol_ctl = vol_ctl;
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
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 35
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxInputImpedance(0);   // 10 K Ohms 
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::setTxInputImpedance(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.RIN = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
  this->reg_vga = reg_vga;
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
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 35
 * @details Example
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxDigitalGain(2);   // 2dB 
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::setTxDigitalGain(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.TXAGC_GDB = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
  this->reg_vga = reg_vga;
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
 * @param value  
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 35
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setsetTxInputImpedance(0);  // Sets the input impedance to 10 Kohms
 *   tx.setTxInputBufferGain(5);    // Now the Gain will be 18dB
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
void QN8066::setTxInputBufferGain(uint8_t value) {
  qn8066_reg_vga reg_vga; 

  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.TXAGC_GVGA = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);
  this->reg_vga = reg_vga;
}


/**
 * @ingroup group04  TX Setup
 * @brief TX soft clipping enable
 * @param value  (true = enabled; false = disabled)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxSoftClippingEnable(true);  // Enabled
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
void QN8066::setTxSoftClippingEnable( bool value) {
  qn8066_reg_vga reg_vga; 
  reg_vga.raw = this->getRegister(QN_REG_VGA);
  reg_vga.arg.tx_sftclpen = value;
  this->setRegister(QN_REG_VGA, reg_vga.raw);  
  this->reg_vga = reg_vga;
}


/**
 * @ingroup group04  TX Setup
 * @brief Specify total TX frequency deviation.
 * @details TX frequency deviation = 0.69KHz*TX_FEDV. The default value is 108 (Binary: 01101100)
 * @param value  
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setTxFrequencyDerivation(120);  // About +- 84 kHz
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
 void QN8066::setTxFrequencyDerivation(uint8_t value) {
  this->fdev.raw = value; 
  this->setRegister(QN_FDEV, value );
 }

/**
 * @ingroup group04 PA Control
 * @brief PA output power target control.
 * @details PA output power target is 0.91*PA_TRGT+70.2dBu. .
 * @param pacValue
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setPAC(56);  // 
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::setPAC(uint8_t PA_TRGT) {

  this->pac.raw = this->getRegister(QN_PAC);
  this->pac.arg.PA_TRGT = PA_TRGT;
  this->pac.arg.TXPD_CLR = !(this->pac.arg.TXPD_CLR); // Reset aud_pk ( Toggle the value)

  if ( PA_TRGT > 23 ) { 
    this->setRegister(QN_PAC, this->pac.raw );
  }
}

/**
 * @ingroup group04 PA Control
 * @brief   Reset the system kepping the TX current STATUS. 
 * @details Some functions do not affect the system when the TX mode is on. In this case, you must use these functions after configuring certain parameters.
 * @todo Make it work - Under construction
 */
void QN8066::updateTxSetup() {

   // UNDER CONSTRUCTION... 
   // Save the current register status 
   this->system2.raw = this->getRegister(QN_SYSTEM2); 
   this->rds.raw = this->getRegister(QN_RDS); 
   this->txch.raw = this->getRegister(QN_TXCH); 
   this->cca.raw = this->getRegister(QN_CCA);   
   this->int_ctrl.raw = this->getRegister(QN_INT_CTRL);  
   this->fdev.raw =  this->getRegister(QN_FDEV);
   this->xtal_div0.raw = this->getRegister(QN_XTAL_DIV0);
   this->xtal_div1.raw = this->getRegister(QN_XTAL_DIV1);
   this->xtal_div2.raw = this->getRegister(QN_XTAL_DIV2);
   this->reg_vga.raw = this->getRegister(QN_REG_VGA);
   this->gplt.raw = this->getRegister(QN_GPLT);
   this->pac.raw = this->getRegister(QN_PAC);

   this->setRegister(QN_SYSTEM1, this->system1.raw); // RESET the system

   this->setRegister(QN_SYSTEM2, this->system2.raw);  
   this->system2.arg.rdsrdy = !(this->system2.arg.rdsrdy); // Toggle 
   this->setRegister(QN_SYSTEM2, this->system2.raw); 

   this->setRegister(QN_CCA, this->cca.raw);  
   this->setRegister(QN_XTAL_DIV0, this->xtal_div0.raw);
   this->setRegister(QN_XTAL_DIV1, this->xtal_div1.raw);
   this->setRegister(QN_XTAL_DIV2, this->xtal_div2.raw);

   this->setRegister(QN_SYSTEM1, this->system1.raw); 

   this->setRegister(QN_FDEV,this->fdev.raw);
   this->setRegister(QN_RDS, this->rds.raw);
   this->setRegister(QN_GPLT,this->gplt.raw);   
   this->setRegister(QN_PAC, this->pac.raw);

   // Sets the previous frequency
   this->setRegister(QN_INT_CTRL,this->int_ctrl.raw);
   this->setRegister(QN_TXCH,this->txch.raw);

  // Checking unkown registers
  // this->setRegister(0x49, 0B11101000); 
  this->setRegister(0x49, 0B11011111); 
  this->setRegister(0x6E, 0B11111111); 

  this->setRegister(QN_SYSTEM1, this->system1.raw); 
  this->setRegister(QN_REG_VGA, this->reg_vga.raw); // REG_VGA =>  01011011 => Tx_sftclpen = 0; TXAGC_GVGA = 101; TXAGC_GDB = 10; RIN = 11 (80K)

};

/**
 * @ingroup group04 PA Control
 * @brief TX Audio peak clear signal.
 * @details Audio peak value is max-hold and stored in aud_pk (see STATUS register). Once TXPD_CLR is toggled, the aud_pk value is cleared and restarted again
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.setToggleTxPdClear();   
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::setToggleTxPdClear() {
  qn8066_pac pac;
  pac.raw = this->getRegister(QN_PAC);
  pac.arg.TXPD_CLR = !pac.arg.TXPD_CLR;
  this->setRegister(QN_PAC, pac.raw );
  this->pac = pac;
}


/**
 * @ingroup group04 PA Control
 * @brief Audio peak value at ADC input
 * @details  Audio peak value at ADC input is aud_pk * 45mV. See STATUS3 register.
 * @return integer value result of  aud_pk * 45 (in mV).
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   int audioPeak = tx.getAudioPeakValue();   // Stores the current audio peak value
 *   ...
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
int QN8066::getAudioPeakValue() {
  return this->getStatus3().arg.aud_pk * 45;
}


/** @defgroup group05 TX RDS Setup */

/**
 * @ingroup group05 TX RDS 
 * @brief Sets RDS Mode Selection.
 * @details if 0, Received bit-stream have both RDS and MMBS blocks (‘E’ block); 
 * @details if 1, Received bit-stream has RDS block only, no MMBS block (‘E’ block)
 * @param mode
 * @details Example 
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsSetMode(0);
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::rdsSetMode(uint8_t mode) {
  qn8066_int_ctrl int_ctrl;
  int_ctrl.raw = this->getRegister(QN_INT_CTRL);
  int_ctrl.arg.rds_only = mode;
  this->setRegister(QN_INT_CTRL, int_ctrl.raw );
}

/**
 * @ingroup group05 TX RDS 
 * @brief Sets RDS 4K Mode .
 * @details Enable RDS RX/TX 4k Mode: with or without the privacy mode (audio scramble and RDS encryption)
 * @param value 0 or 1 
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsSet4KMode(0);
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::rdsSet4KMode(uint8_t value) {
  qn8066_int_ctrl int_ctrl;
  int_ctrl.raw = this->getRegister(QN_INT_CTRL);
  int_ctrl.arg.rds_4k_mode = value;
  this->setRegister(QN_INT_CTRL, int_ctrl.raw );
}

/**
 * @ingroup group05 TX RDS 
 * @brief Sets RDS interrupt
 * @details RDS RX interrupt enable. When RDS_INT_EN=1, a 4.5ms low pulse will be output from pad din (RX mode)
 * @details when a new group data is received and stored into RDS0~RDS7 (RX mode).
 * @param value 0 or 1 
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsSetInterrupt(0);
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::rdsSetInterrupt(uint8_t value) {
  qn8066_int_ctrl int_ctrl;
  int_ctrl.raw = this->getRegister(QN_INT_CTRL);
  int_ctrl.arg.rds_int_en = value;
  this->setRegister(QN_INT_CTRL, int_ctrl.raw );
}


/**
 * @ingroup group05 TX RDS 
 * @brief Sets some RDS parameters
 * @param countryId - Country Identifier (First 4 Bits)
 * @param programId - Program Id code
 * @param reference - Program Reference Number  (8 bits). It  provides a unique reference number for the specific station or program.
 * @param pty       - Program type (PTY) - Default is 1 (News)
 * @param rdsSyncTime - Time in ms to wait for sending the next group - Default value is 60 ms 
 * @param rdsRepeatGroup -  Number of times that a RDS group will send at once. - Default is 5.
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * 
 * uint8_t countryId = 0;
 * uint8_t programId = 0;
 * uint8_t reference = 0;
 * 
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsInitTx(countryId, programId, reference);
 * }
 *
 * void loop() {
 * }
 * @endcode  
 * @see rdsSetPI, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup 
 */
void QN8066::rdsInitTx(uint8_t countryId, uint8_t programId, uint8_t reference, uint8_t pty, uint8_t rdsSyncTime, uint8_t rdsRepeatGroup) {
  // this->setRegister(0x6E, 0B10110111);  // TEST - Stop Auto Gain Correction (AGC)
  this->rdsSetPI(countryId, programId, reference );
  this->rdsSyncTime = rdsSyncTime; 
  this->rdsRepeatGroup = rdsRepeatGroup;
  this->rdsPTY = pty;
  delay(100);
}

/**
 * @ingroup group05 TX RDS
 * @brief Sets the Program Identification (PI)
 * @param countryId - Country Identifier (First 4 Bits)
 * @param programId - Program Id code
 * @param reference - Program Reference Number  (8 bits). It  provides a unique reference number for the specific station or program.
 * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup
 */
void QN8066::rdsSetPI(uint8_t countryId, uint8_t programId, uint8_t reference) {
  RDS_BLOCK1 pi; 
  pi.field.countryId = countryId;
  pi.field.programId = programId;
  pi.field.reference = reference;
  this->rdsPI = pi.pi;
};

/**
 * @ingroup group05 TX RDS
 * @brief Transmitter RDS enable
 * @details Enable RDS service 
 * @param value (true = enabled; false = disabled)
 * @see  Pages 20 and 21 of the Datasheet (Register SYSTEM2)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 * }
 *
 * void loop() {
 * }
 * @endcode   
 * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup 
 */
void QN8066::rdsTxEnable(bool value) {
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
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   ...
 *   tx.rdsSetTxToggle();
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
uint8_t QN8066::rdsSetTxToggle() {
  this->system2.raw = this->getRegister(QN_SYSTEM2);
  this->system2.arg.rdsrdy = !(this->system2.arg.rdsrdy);
  this->setRegister(QN_SYSTEM2, this->system2.raw);
  return this->system2.arg.rdsrdy;
}

/**
 * @ingroup group05 TX RDS
 * @brief RDS TX Updated
 * @details To transmit the 8 bytes in RDS0~RDS7, user should toggle the register bit RDSRDY (See SYSTEM2 register).
 * @details Then the chip internally fetches these bytes after completing transmitting of current group.
 * @details Once the chip internally fetched these bytes, it will toggle this bit, and user can write in another group.
 * @return true 
 * @return false 
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true); 
 *   ...
 *   tx.rdsGetTxUpdated();
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
bool QN8066::rdsGetTxUpdated() { 
    return this->getStatus3().arg.RDS_TXUPD;
}

/**
 * @ingroup group05 TX RDS
 * @brief Sets RDS frequency deviation.
 * @details RDS frequency deviation = 0.35KHz*RDSFDEV in normal mode. 
 * @details RDS frequency deviation = 0.207KHz*RDSFDEV in 4k mode and private mode.
 * @param freq ( valid values: from 0 to 127)
 * @see Datasheet, register RDS (0x26), page 34.
 * @details Example 
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   tx.rdsSetFrequencyDerivation();
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
void QN8066::rdsSetFrequencyDerivation(uint8_t freq) {
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
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   tx.rdsSetTxLineIn(false); 
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
void QN8066::rdsSetTxLineIn(bool value) {
  qn8066_rds rds;
  rds.raw = this->getRegister(QN_RDS);
  rds.arg.line_in_en = value;
  this->setRegister(QN_RDS, rds.raw);  
} 

/**
 * @ingroup group05 TX RDS
 * @brief Clear and Flushes the RDS Buffer
 * 
 */
void QN8066::rdsClearBuffer() {

  uint8_t toggle  = this->rdsGetTxUpdated(); 
  uint8_t count = 0;  

  for (uint8_t i = QN_TX_RDSD0; i <= QN_TX_RDSD7; i++ )
    this->setRegister(i, 0 );

  delay(87); 
  // checks for the RDS_TXUPD . 
  while ( this->rdsGetTxUpdated() == toggle  && count < 10) { 
    delay(1);
    count++;
  }

  if (count >= 10 ) 
    this->rdsSendError = 1;

};

/**
 * @ingroup group05 TX RDS
 * @brief Sends a RDS group (four blocks)  to the QN8066
 * @details Each block is packaged in 16 bits word (two bytes)
 * @param block1 - RDS_BLOCK1 datatype
 * @param block2 - RDS_BLOCK2 datatype
 * @param block3 - RDS_BLOCK3 datatype
 * @param block4 - RDS_BLOCK4 datatype 
 */
void QN8066::rdsSendGroup(RDS_BLOCK1 block1, RDS_BLOCK2 block2, RDS_BLOCK3 block3, RDS_BLOCK4 block4) {

  uint8_t toggle  = this->rdsGetTxUpdated(); 
  uint8_t count = 0;

  this->rdsSendError = 0;

  this->setRegister(QN_TX_RDSD0, block1.byteContent[1]); // Most Significant Byte First.
  this->setRegister(QN_TX_RDSD1, block1.byteContent[0]);

  this->setRegister(QN_TX_RDSD2, block2.byteContent[1]); // Most Significant Byte First.
  this->setRegister(QN_TX_RDSD3, block2.byteContent[0]);
  
  this->setRegister(QN_TX_RDSD4, block3.byteContent[0]); // First character first 
  this->setRegister(QN_TX_RDSD5, block3.byteContent[1]);
  
  this->setRegister(QN_TX_RDSD6, block4.byteContent[0]);
  this->setRegister(QN_TX_RDSD7, block4.byteContent[1]);

  // It should not be here. Judiging by the data sheet, the use must  
  // wait for the RDS_TXUPD before toggling the RDSRDY bit in the SYSTEM2 register. 
  this->rdsSetTxToggle(); 
  delay(this->rdsSyncTime); // This time is very critical and may need to be tuned. Check the function/method rdsSetSyncTime 
  // checks for the RDS_TXUPD . 
  while ( this->rdsGetTxUpdated() == toggle  && count < 10) { 
    delay(1);
    count++;
  }
  if (count >= 10 ) 
    this->rdsSendError = 1;
}

/**
 * @ingroup group05 TX RDS
 * @brief Sets the station name 
 * 
 * @param stationName 
 */
void QN8066::rdsSetStationName(char *stationName) { 
  strncpy(this->rdsStationName,stationName,8);
  rdsStationName[8] = '\0';
}

/**
 * @ingroup group05 TX RDS
 * @brief Sends the Program Service Message
 * @details Like rdsSendPS this method sends the Station Name or other 8 char message.
 * @details This function repeats sending a group this->rdsRepeatGroup times.
 * @param ps - String with the name of Station or message limeted to 8 character.
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   delay(100);
 *   tx.rdsSendPS("STATIONX"); // transmit "STATIONX" this->rdsRepeatGroup times
 * }
 *
 * void loop() {
 * }
 * @endcode    
 */
void QN8066::rdsSendPS(char* ps) {

  RDS_BLOCK1 b1;
  RDS_BLOCK2 b2;
  RDS_BLOCK3 b3;
  RDS_BLOCK4 b4;

  this->rdsSetStationName(ps);

  b1.pi = this->rdsPI;

  b2.raw = 0; // Starts block2
  b2.group0Field.address = 0;
  b2.group0Field.DI = 0;
  b2.group0Field.MS = 0;
  b2.group0Field.TA = 0;
  b2.group0Field.programType = this->rdsPTY;
  b2.group0Field.trafficProgramCode = this->rdsTP;  
  b2.group0Field.versionCode = 1; // 0B - Station Name
  b2.group0Field.groupType = 0;  
  b3.raw = b1.pi;
  // Sending the packet only once did not work for some types of receivers with RDS support. 
  // Therefore, through trial and error, transmitting the same RT message three or more times
  // made  this function works. 
  // It is important to ensure that the 2A or 2B groups are transmitted continuously and in 
  // sync so that receivers can correctly piece together the parts of the text and display 
  // them to the listener without interruptions.
  for ( uint8_t k  = 0; k < this->rdsRepeatGroup; k++) { 
    for (uint8_t i = 0; i < 8; i+=2) { 
      b4.byteContent[0] = ps[i]; 
      b4.byteContent[1] = ps[i+1];
      this->rdsSendGroup(b1, b2, b3, b4);
      b2.group0Field.address++; 
    }
  } 

}

/**
 * @ingroup group05 TX RDS
 * @brief Sends RDS Radio Text Message (group 2A)
 * @details This function repeats sending a group this->rdsRepeatGroup times.
 * @param rt - Radio Text (string of 32 character)
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   delay(100);
 *   tx.rdsSendRTMessage("IT IS AN EXAMPLE..."); // transmits the message "IT IS AN EXAMPLE..." this->rdsRepeatGroup times
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
void QN8066::rdsSendRTMessage(char *rt) {

    // Flushes any previus data
    this->rdsSetTxToggle();

    int textLen = strlen(rt);
    int numGroups = (textLen + 3) / 4; // Each group can contain 4 characters
    RDS_BLOCK1 block1;
    block1.pi = this->rdsPI;
    static bool toggle = false;

    toggle = !toggle;

    RDS_BLOCK2 block2; 
    block2.raw = 0;
    block2.group2Field.textABFlag = toggle;
    block2.group2Field.programType = this->rdsPTY;
    block2.group2Field.trafficProgramCode = this->rdsTP;
    block2.group2Field.versionCode = 0; // Version A
    block2.group2Field.groupType = 2;  // Group 2

    // Sending the packet only once did not work for some types of receivers with RDS support. 
    // Therefore, through trial and error, transmitting the same RT message three or more times
    // made  this function feasible.
    // It is important to ensure that the 2A or 2B groups are transmitted continuously and in 
    // sync so that receivers can correctly piece together the parts of the text and display 
    // them to the listener without interruptions.    
    for ( uint8_t k  = 0; k < this->rdsRepeatGroup; k++) { 
      for (uint8_t i = 0; i < numGroups; i++) {
          block2.group2Field.address = i; 
          RDS_BLOCK3 block3; 
          block3.byteContent[0] = rt[i * 4];
          block3.byteContent[1] = rt[i * 4 + 1];
          RDS_BLOCK4 block4;
          block4.byteContent[0] = rt[i * 4 + 2];
          block4.byteContent[1] = rt[i * 4 + 3]; 
          this->rdsSendGroup(block1, block2, block3, block4);
      }
    }
}


/** @defgroup group10 QN8066 FSM functions **/

/**
 * @ingroup group10 FSM Status
 * @brief Get the FSM State Code 
 * @return uint8_t - Current FSM code
 * @details The table below shows the FSM status and description
 * | FSM Status | Description  |
 * | ---------- | ------------ |
 * |  0 - 0000  | STBY         |
 * |  1 - 0001  | RESET        |
 * |  2 - 0010  | CALI         |
 * |  3 - 0011  | IDLE         |
 * |  4 - 0100  | CALIPLL      |
 * |  5 - 0101  | Reserved     |
 * |  6 - 0110  | Reserved     |
 * |  7 - 0111  | TXPLLC       |
 * |  8 - 1000  | TX_RSTB      |
 * |  9 - 1001  | PACAL        |
 * | 10 - 1010  | TRANSMIT     |
 * | 11 - 1011  | TXCCA        |
 * | Others     | Reserved     |
 * @details Example
 * @code 
 * #include <QN8066.h>
 * QN8066 tx;
 * int nFSM;
 * void setup() {
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.rdsTxEnable(true);
 *   nFSM = tx.getFsmStateCode(); 
 *   ...
 * }
 *
 * void loop() {
 * }
 * @endcode   
 */
uint8_t QN8066:: getFsmStateCode() {
  qn8066_status1 s1;
  s1.raw = this->getRegister(QN_STATUS1);
  return s1.arg.FSM;
}

/**
 * @ingroup group10 FSM RESET
 * @brief Resets the state to initial states and recalibrate all blocks
 * @details Reset the FSM. After this bit is de-asserted, FSM will go through all the power up and calibration sequence.
 */
void QN8066::resetFsm() {
  qn8066_system1 system1;
  system1.raw = this->getRegister(QN_SYSTEM1);
  system1.arg.recal = 1;
  this->setRegister(QN_SYSTEM1, system1.raw);
}

/**
 * @ingroup group10 FSM RESET
 * @brief Request Immediately enter Standby mode whatever state chip is in.
 * @details Note: “stnby” has the highest priority. 
 * @param value - true or false
 */
void QN8066::setStnby(bool value) {
  qn8066_system1 system1;
  system1.raw = this->getRegister(QN_SYSTEM1);
  system1.arg.stnby = value;
  this->setRegister(QN_SYSTEM1, system1.raw);
}


/** @defgroup group99 Helper and Tools functions*/

/**
 * @ingroup group99 Covert numbers to char array
 * @brief Converts a number to a char array
 * @details It is useful to mitigate memory space used by functions like sprintf or other generic similar functions
 * @details You can use it to format frequency using decimal or thousand separator and also to convert small numbers.
 *
 * @param value  value to be converted
 * @param strValue char array that will be receive the converted value
 * @param len final string size (in bytes)
 * @param dot the decimal or thousand separator position
 * @param separator symbol "." or ","
 * @param remove_leading_zeros if true removes up to two leading zeros (default is true)
 * @code
 * #include <QN8066.h>
 * QN8066 tx;
 * char strFrequency[7];
 * uint16_t txFrequency = 1069;
 * void setup() {
 *   char strFrequency[7];
 *   tx.setup();
 *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
 *   tx.convertToChar(txFrequency, strFrequency, 5, 3, ','); // Convert 1069 to a array of char "106.9"
 * }
 *
 * void loop() {
 * }
 * @endcode
 * 
 */
void QN8066::convertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator, bool remove_leading_zeros)
{
    char d;
    for (int i = (len - 1); i >= 0; i--)
    {
        d = value % 10;
        value = value / 10;
        strValue[i] = d + 48;
    }
    strValue[len] = '\0';
    if (dot > 0)
    {
        for (int i = len; i >= dot; i--)
        {
            strValue[i + 1] = strValue[i];
        }
        strValue[dot] = separator;
    }

    if (remove_leading_zeros)
    {
        if (strValue[0] == '0')
        {
            strValue[0] = ' ';
            if (strValue[1] == '0')
                strValue[1] = ' ';
        }
    }
}
