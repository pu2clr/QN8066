/**
 * @brief QN8066 ARDUINO LIBRARY
 *
 * @details This is an Arduino library for the QN8066 FM RX/TX device (Digital FM Transceiver for Portable Devices).
 * @details The communication used by this library is I2C.
 * @details This file contains: const (#define), Defined Data type and Methods declarations 
 * @details You can see a complete documentation on  <https://github.com/pu2clr/QN8066>
 * @details There are examples that can help you in your project on  <https://github.com/pu2clr/QN8066/tree/master/examples>
 * @see [General Documentation](https://pu2clr.github.io/QN8066/)
 *
 * @author PU2CLR - Ricardo Lima Caratti
 * @date  2024
 */

#ifndef _QN8066_H // Prevent this file from being compiled more than once
#define _QN8066_H

#include <Arduino.h>
#include <Wire.h>

#define QN8066_I2C_ADDRESS 0x21   // See Datasheet pag. 16.
#define QN8066_RESET_DELAY 1000   // Delay after reset in us
#define QN8066_DELAY_COMMAND 2500 // Delay after command

/**
 * @brief QN8066 Register addresses
 *
 */
#define QN_SYSTEM1 0x00   //<! Sets device modes.
#define QN_SYSTEM2 0x01   //<! Sets external clock type and CCA parameters.
#define QN_CCA 0x02       //<! Sets CCA parameters.
#define QN_SNR 0x03       //<! Estimate RF input CNR value
#define QN_RSSISIG 0x04   //<! In-band signal RSSI dBµ V value.
#define QN_CID1 0x05      //<! Device ID numbers.
#define QN_CID2 0x06      //<! Device ID numbers.
#define QN_XTAL_DIV0 0x07 //<! Frequency select of reference clock source.
#define QN_XTAL_DIV1 0x08 //<! Frequency select of reference clock source.
#define QN_XTAL_DIV2 0x09 //<! Frequency select of reference clock source.
#define QN_STATUS1 0x0A   //<! System status.
#define QN_RX_CH 0x0B     //<! Lower 8 bit of 10-bit receiver channel index.
#define QN_CH_START  0x0C //<! Lower 8 bits of 10-bit channel scan start channel index.
#define QN_CH_STOP   0x0D //<! Lower 8 bits of 10-bit channel scan stop channel index.
#define QN_CH_STEP   0x0E //<! Channel scan frequency step. Highest 2 bits of receiver channel indexes.
#define QN_RX_RDSD0 0x0F //<! RDS data byte 0.
#define QN_RX_RDSD1 0x10 //<! RDS data byte 1.
#define QN_RX_RDSD2 0x11 //<! RDS data byte 2.
#define QN_RX_RDSD3 0x12 //<! RDS data byte 3.
#define QN_RX_RDSD4 0x13 //<! RDS data byte 4.
#define QN_RX_RDSD5 0x14 //<! RDS data byte 5.
#define QN_RX_RDSD6 0x15 //<! RDS data byte 6.
#define QN_RX_RDSD7 0x16 //<! RDS data byte 7.
#define QN_STATUS2 0x17  //<! Receiver RDS status indicators.
#define QN_VOL_CTL 0x18  //<! Audio volume control.
#define QN_INT_CTRL 0x19 //<! Receiver RDS control
#define QN_STATUS3 0x1A  //<! Receiver audio peak level and AGC status.
#define QN_TXCH 0x1B     //<! Lower 8 bit of 10-bit transmitter channel index.
#define QN_TX_RDSD0 0x1C //<! Transmit RDS data byte0.
#define QN_TX_RDSD1 0x1D //<! Transmit RDS data byte1.
#define QN_TX_RDSD2 0x1E //<! Transmit RDS data byte2.
#define QN_TX_RDSD3 0x1F //<! Transmit RDS data byte3.
#define QN_TX_RDSD4 0x20 //<! Transmit RDS data byte4
#define QN_TX_RDSD5 0x21 //<! Transmit RDS data byte5
#define QN_TX_RDSD6 0x22 //<! Transmit RDS data byte6
#define QN_TX_RDSD7 0x23 //<! Transmit RDS data byte7
#define QN_PAC 0x24      //<! PA output power target control.
#define QN_FDEV 0x25     //<! Specify total TX frequency deviation.
#define QN_RDS 0x26      //<! Specify transmit RDS frequency deviation.
#define QN_GPLT 0x27     //<! Transmitter soft chip threshold, gain of TX pilot.
#define QN_REG_VGA 0x28  //<! TX AGC gain.
#define QN_REGISTER_6E 0x6E //<! This register is not documented in the Data Sheet. However, according to tests and observations, it seems to affect the quality and stability of the audio.
#define QN_REGISTER_49 0x49 //<! This register is not documented in the Data Sheet. However, according to observations, it makes the system more stable (not sure)  

/** @defgroup group00 Union, Struct and Defined Data Types
 * @section group01 Data Types
 *
 * @brief QN8066 data representation
 *
 * @details
 *
 */

/**
 * @ingroup group00
 *
 * @brief System1 - Sets device modes (Address: 00h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 19
 */

typedef union {
  struct {
    uint8_t cca_ch_dis : 1; //!<  0 = RX_CH is decided by internal CCA; 1 =  RX_CH is decided writing in RX_CH[9:0]
    uint8_t ccs_ch_dis : 1; //!<  0 = TX_CH is decided by internal CCS; 1 =  TX_CH is decided writing in TX_CH[9:0]
    uint8_t chsc : 1;  //!< Channel Scan mode enable - 0 = Normal operation; 1 = Channel Scan mode operation
    uint8_t txreq : 1; //!< Transmission request - 0 = Non TX mode; 1 = Enter transmit mode
    uint8_t rxreq : 1; //!< Receiving request - 0 = Non RX mode; 1 = Enter Receiving mode
    uint8_t stnby : 1; //!< Request Immediately enter Standby mode whatever state chip is in - 0 = Non standby mode; 1 = Enter standby mode
    uint8_t recal : 1; //!< Reset the state to initial states and recalibrate all blocks - 0 = No action. FSM runs normally;  1 =  Reset the FSM. After this bit is de-asserted, FSM will go through all the power up and calibration sequence. 
    uint8_t swrst : 1; //!< Reset all registers to default values; 0 = Keep the current value; 1 = Reset to default values
  } arg;
  uint8_t raw;
} qn8066_system1;

/**
 * @ingroup group00
 *
 * @brief System2 - Sets device modes (Address: 01h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 20
 */

typedef union {
  struct {
    uint8_t tc : 1;       //!<  Pre-emphasis and de-emphasis time constant; 0 = 50; 1 = 75 
    uint8_t rdsrdy : 1;   //!<  RDS transmitting ready; - If user want the chip transmitting all the 8 bytes in RDS0~RDS7, user  should toggle this bit.
    uint8_t tx_mute : 1;  //!<  TX audio mute enabel - 0 = Mute Disabled; 1 = Mute Enabled; 
    uint8_t rx_mute : 1;  //!<  RX audio Mute enable - 0 = Mute Disabled; 1 = Mute Enabled
    uint8_t tx_mono : 1;  //!<  TX stereo and mono mode selection; 0 = Stereo;  1 = Mono
    uint8_t force_mo : 1; //!<  Force receiver in MONO mode; 0 = Not forced. ST/MONO auto selected; Forced in MONO mode
    uint8_t tx_rdsen : 1; //!<  Transmitter RDS enable; 0 = RDS Disable; 1 = RDS  Enable
    uint8_t rx_rdsen : 1; //!<  Receiver RDS enable; 0 = RDS Disable; 1 = RDS Enable
  } arg;
  uint8_t raw;
} qn8066_system2;

/**
 * @ingroup group00
 *
 * @brief CCA - Sets CCA parameters ( Address: 02h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 21
 */

typedef union {
  struct {
    uint8_t SNR_CCA_TH : 6; //!<  The threshold for determination of whether  current channel is valid by check its SNR.
    uint8_t imr : 1;        //!<  Image Rejection. 0 = LO<RF, image is in lower side; 1 = LO>RF, image is in upper side
    uint8_t xtal_inj : 1;   //!<  Select the reference clock source. 0 = Inject sine-wave clock; 1 = Inject digital clock
  } arg;
  uint8_t raw;
} qn8066_cca;

/**
 * @ingroup group00
 *
 * @brief SRN - Estimate RF input CNR value( Address: 03h - Read Only)
 * @details Estimated RF input SNR.
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 21
 */

typedef union {
  uint8_t SNRDB;
  uint8_t raw;
} qn8066_snr;

/**
 * @ingroup group00
 *
 * @brief RSSISIG - In-band signal RSSI (Received signal strength indicator)
 * dBuV value. dBuV=RSSI-49( Address: 04h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 22
 */

typedef union {
  uint8_t RSSISIG;
  uint8_t raw;
} qn8066_rssisig;

/**
 * @ingroup group00
 *
 * @brief CID1 - Device ID numbers ( Address: 05h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 22
 */

typedef union {
  struct {
    uint8_t CID2 : 2; //!<  Chip ID for minor revision: 1~4
    uint8_t CID1 : 3; //!<  Chip ID for product family: 000 = FM; others Reserved
    uint8_t RSVD : 3; //!<  Reserved
  } arg;
  uint8_t raw;
} qn8066_cid1;

/**
 * @ingroup group00
 *
 * @brief CID2 - Device ID numbers ( Address: 06h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 22
 */

typedef union {
  struct {
    uint8_t CID4 : 2; //!<  Sequency integer values from 0 to 4.
    uint8_t CID3 : 6; //!<  Chip ID for product ID. 001101 = Transceiver –  QN8066; Others = Reserved unkown
  } arg;
  uint8_t raw;
} qn8066_cid2;

/**
 * @ingroup group00
 *
 * @brief XTAL_DIV0 - Frequency select of reference clock source (Lower bits -
 * Address: 07h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 23
 */

typedef union {
  uint8_t xtal_div; // !< Lower 8 bits of xtal_div[10:0]. Xtal_div[10:0] = round(freq of xtal/32.768KHz).
  uint8_t raw;
} qn8066_xtal_div0;

/**
 * @ingroup group00
 *
 * @brief XTAL_DIV1 - Frequency select of reference clock source (Lower bits -
 * Address: 08h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 23
 */

typedef union {
  struct {
    uint8_t xtal_div : 3; //!<  Higher 3 bits of xtal_div[10:0]. Xtal_div[10:0] = round(freq of xtal/32.768KHz)
    uint8_t pll_dlt : 5;  //!<  Lower 5 bits of pll_dlt[12:0].
  } arg;
  uint8_t raw;
} qn8066_xtal_div1;

/**
 * @ingroup group00
 *
 * @brief XTAL_DIV2 - Frequency select of reference clock source (Lower bits -
 * Address: 09h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 23
 */

typedef union {
  uint8_t pll_dlt;
  uint8_t raw;
} qn8066_xtal_div2;

/**
 * @ingroup group00
 *
 * @brief STATUS1 -  System status ( Address: 0Ah - Read Only)
 *
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
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 24
 */

typedef union {
  struct {
    uint8_t ST_MO_RX : 1; //!<< Stereo receiving status. 0 = Stereo; 1 = Mono
    uint8_t RXSTATUS : 1; //!<< RX Status. 0 = No receiving; 1 = Receiving
    uint8_t RXAGCSET : 1; //!<< RX AGC Settling status. 0 = Not settled; 1 = Settled
    uint8_t rxcca_fail : 1; //!<< RXCCA status flag. To indicate whether a valid channel is found during RX CCA. 0 = RX CCA success to find a valid channel; 1 = RX CCA fail to find a valid channel
    uint8_t FSM : 4;        //!<< Top FSM state code
  } arg;
  uint8_t raw;
} qn8066_status1;

/**
 * @ingroup group00
 *
 * @brief   RX_CH - Lower 8 bit of 10-bit receiver channel index (Address: 0Bh -
 * Write Only)
 * @details Channel used for RX have two origins, one is from RXCH register
 * (REG0EH[1:0]+REG0BH)
 * @details which can be written by the user, another is from CCA. CCA selected
 * channel is stored in an internal register, which is
 * @details physically a different register with CH register, but it can be read
 * out through register CH and be used for RX when
 * @details CCA_CH_DIS(REG0[0])=0.
 * @details FM channel: (60+RXCH*0.05)MHz
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 25
 */

typedef union {
  uint8_t RXCH;
  uint8_t raw;
} qn8066_rx_ch;

/**
 * @ingroup group00
 *
 * @brief CH_START - Lower 8 bits of 10-bit CCA(channel scan) start channel
 * index (Address: 0Ch - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 25
 */

typedef union {
  uint8_t CH_START;
  uint8_t raw;
} qn8066_ch_start;

/**
 * @ingroup group00
 *
 * @brief CH_STOP - Lower 8 bits of 10-bit channel scan stop channel index
 * (Address: 0Dh - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 25
 */

typedef union {
  uint8_t CH_STOP;
  uint8_t raw;
} qn8066_ch_stop;

/**
 * @ingroup group00
 *
 * @brief CH_STEP - Channel scan frequency step (Address: 0Eh - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 26
 */

typedef union {
  struct {
    uint8_t RXCH : 2; //!< Highest 2 bits of 10-bit channel index. Channel freq is (60+RXCH*0.05)MHz
    uint8_t CH_STA : 2; //!< Highest 2 bits of 10-bit CCA(channel scan) start channel index. Start freq is (60+RXCH_STA*0.05)MHz
    uint8_t CH_STP : 2; //!< Highest 2 bits of 10-bit CCA(channel scan) stop channel index. Stop freq is (60+RXCH_STP*0.05)MHz
    uint8_t CH_FSTEP : 2; //!< CCA (channel scan) frequency step. 00=50kHz; 01=100kHz; 10 = 200kHz; 11=Reserved
  } arg;
  uint8_t raw;
} qn8066_ch_step;

/**
 * @ingroup group00
 *
 * @brief RDS - RDS data byte 0 to byte 7 (Address: 0Fh to 16h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 26-28
 */

typedef union {
  struct {
    uint8_t RX_RDSD0; //!< RDS data byte 0  - 0Fh
    uint8_t RX_RDSD1; //!< RDS data byte 1  - 10h
    uint8_t RX_RDSD2; //!< RDS data byte 2  - 11h
    uint8_t RX_RDSD3; //!< RDS data byte 3  - 12h
    uint8_t RX_RDSD4; //!< RDS data byte 4  - 13h
    uint8_t RX_RDSD5; //!< RDS data byte 5  - 14h
    uint8_t RX_RDSD6; //!< RDS data byte 6  - 15h
    uint8_t RX_RDSD7; //!< RDS data byte 7  - 16h
  } arg;
  uint8_t data[8];
} qn8066_rx_rds;

/**
 * @ingroup group00
 *
 * @brief STATUS2 - Receiver RDS status indicators (Address: 17h - Read Only)
 *
 * @details RDS_RXUPD - RDS RX: RDS received group updated. Each time a new
 * group is received, this bit will be toggled.
 * @details             If RDS_INT_EN=1, then at the same time this bit is
 * toggled, interrupt output will out put a 4.5 ms low pulse
 * @details             0->1 or 1->0 -> A new set (8 Byte) of data is received
 * @details             0->0 or 1->1 -> New data is in receiving
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 28-29
 */

typedef union {
  struct {
    uint8_t RDS3ERR : 1; //!< 0 = No Error
    uint8_t RDS2ERR : 1; //!< 0 = No Error
    uint8_t RDS1ERR : 1; //!< 0 = No Error
    uint8_t RDS0ERR : 1; //!< 0 = No Error
    uint8_t RDSSYNC : 1; //!< RDS block synchronous indicator. 0 =  Non-synchronous;  1 = Synchronous
    uint8_t RDSC0C1 : 1; //!< Type indicator of the RDS third block in one  group. 0 = C0; 1 = C1
    uint8_t E_DET : 1; //!< ‘E’ block (MMBS block) detected. 0 = Not detected; 1 = Detected
    uint8_t RDS_RXUPD : 1; //!< RDS received group updated. Each time a new  group is received, this bit will be toggled. See comment above.
  } arg;
  uint8_t raw;
} qn8066_status2;

/**
 * @ingroup group00
 *
 * @brief VOL_CT -  Audio volume control (Address: 18h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 29
 */

typedef union {
  struct {
    uint8_t GAIN_ANA : 3; //!< set volume control gain of analog portion. From 0 to 7 
    uint8_t GAIN_DIG : 3; //!< set digital volume gain. From 0 to 5
    uint8_t DAC_HOLD : 1; //!< DAC output control. 0 = Normal operation; 1 =  Hold DAC output to a fixed voltage.
    uint8_t TX_DIFF : 1; //!< Tx audio input mode selection. 0 = Single ended; 1  = Differential.
  } arg;
  uint8_t raw;
} qn8066_vol_ctl;

/**
 * @ingroup group00
 *
 * @brief INT_CTRL -  Receiver RDS control (Address: 19h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 30
 */

typedef union {
  struct {
    uint8_t TXCH : 2; //!< Highest 2 bits of 10-bit channel index.  hannel freq
                      //!< is (60+TXCH*0.05)MHz
    uint8_t priv_mode : 1; //!< Private mode for RX/TX
    uint8_t
        rds_4k_mode : 1; //!< Enable RDS RX/TX 4k Mode: with or without the
                         //!< privacy mode (audio scramble and RDS encryption)
    uint8_t s1k_en : 1;  //!< Internal 1K tone selection. It will be used as DAC
                         //!< output when RXREQ. 0 = Disabled; 1 = Enabled
    uint8_t rds_only : 1; //!< RDS Mode Selection. 0 = Received bit-stream have
                          //!< both RDS and MMBS blocks (‘E’ block); 1 =
                          //!< Received bit-stream has RDS block only, no MMBS
                          //!< block (‘E’ block)
    uint8_t cca_int_en : 1; //!< RX CCA interrupt enable. When CCA_INT_EN=1,
                            //!< a 4.5ms low pulse will be output from pad din
                            //!< (RX mode) when a RXCCA (RX mode) is finished. 0
                            //!< = Disabled; 1 = Enabled
    uint8_t
        rds_int_en : 1; //!< RDS RX interrupt enable. When RDS_INT_EN=1, a 4.5ms
                        //!< low pulse will be output from pad din (RX mode)
                        //!< when a new group data is received and stored into
                        //!< RDS0~RDS7 (RX mode). 0 = Disabled; 1 = Enabled
  } arg;
  uint8_t raw;
} qn8066_int_ctrl;

/**
 * @ingroup group00
 *
 * @brief STATUS3 -  Receiver audio peak level and AGC status (Address: 1Ah -
 * Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 30-31
 */

typedef union {
  struct {
    uint8_t rsvd : 1;     //!< Reserved
    uint8_t rxagcerr : 1; //!< RXAGC Error Flag. 0 = No Error; 1 = Error
    uint8_t
        RDS_TXUPD : 1; //!< RDS TX: To transmit the 8 bytes in RDS0~RDS7, user
                       //!< should toggle the register bit RDSRDY. Then the chip
                       //!< internally fetches these bytes after completing
                       //!< transmitting of current group. Once the chip
                       //!< internally fetched these bytes, it will toggle this
                       //!< bit, and user can write in another group.
    uint8_t aud_pk : 4; //!< Audio peak value at ADC input is aud_pk * 45mV
    uint8_t CAP_SH : 1; //!< Large CAP short detection flag. 1 indicates a
                        //!< short. This bit is the OR-ed result of Poly phase
                        //!< filter I path and Poly phase filter Q path.
  } arg;
  uint8_t raw;
} qn8066_status3;

/**
 * @ingroup group00
 *
 * @brief TXCH - Lower 8 bit of 10-bit transmitter channel index (Address: 1Bh -
 * Read and Write)
 * @details Lower 8 bits of 10-bit Channel index. Channel used for TX have two
 * origins, one is from TXCH register (REG19H[1:0]+REG1BH) which can be written
 * by the user,
 * @details another is from CCS. CCS selected channel is stored in an internal
 * register, which is physically a different register with TXCH register, but it
 * can be read out through
 * @details register TXCH and be used for TX when CCS_CH_DIS(REG0[0])=0. FM
 * channel: (60+TXCH*0.05)MHz
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 31
 */

typedef union {
  uint8_t TXCH;
  uint8_t raw;
} qn8066_txch;

/**
 * @ingroup group00
 *
 * @brief RDS - RDS tx data from byte 0 to byte 7 (Address: 1Ch to 23h - Write
 * Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 31-33
 */

typedef union {
  struct {
    uint8_t TX_RDSD0; //!< RDS data byte 0  - 0Fh
    uint8_t TX_RDSD1; //!< RDS data byte 1  - 10h
    uint8_t TX_RDSD2; //!< RDS data byte 2  - 11h
    uint8_t TX_RDSD3; //!< RDS data byte 3  - 12h
    uint8_t TX_RDSD4; //!< RDS data byte 4  - 13h
    uint8_t TX_RDSD5; //!< RDS data byte 5  - 14h
    uint8_t TX_RDSD6; //!< RDS data byte 6  - 15h
    uint8_t TX_RDSD7; //!< RDS data byte 7  - 16h
  } arg;
  uint8_t data[8];
} qn8066_tx_rds;

/**
 * @ingroup group00
 *
 * @brief PAC - PA output power target control (Address: 24h - Write Only)
 * @details  PA_TRGT  - PA output power target is 0.91*PA_TRGT+70.2dBu. Valid
 * values are 24-56.
 * @details  TXPD_CLR -  TX aud_pk clear signal. Audio peak value is max-hold
 * and stored in aud_pk[3:0]. Once TXPD_CLR is toggled, the aud_pk value is
 * cleared and restarted again.
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 33
 */

typedef union {
  struct {
    uint8_t PA_TRGT : 7; //!< PA output power target is 0.91*PA_TRGT+70.2dBu.
                         //!< Valid values are 24-56.
    uint8_t
        TXPD_CLR : 1; //!< TX aud_pk clear signal. Audio peak value is max-hold
                      //!< and stored in aud_pk[3:0]. Once TXPD_CLR is toggled,
                      //!< the aud_pk value is cleared and restarted again.
  } arg;
  uint8_t raw;
} qn8066_pac;

/**
 * @ingroup group00
 *
 * @brief FDEV - Specify total TX frequency deviation (Address: 25h - Write
 * Only)
 * @details Specify total TX frequency deviation. TX frequency deviation =
 * 0.69KHz*TX_FEDV. From 0 to 255
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 33
 */

typedef union {
  uint8_t TX_FDEV; //!< Specify total TX frequency deviation. TX frequency
                   //!< deviation = 0.69KHz*TX_FEDV. From 0 to 255
  uint8_t raw;
} qn8066_fdev;

/**
 * @ingroup group00
 *
 * @brief RDS - Specify transmit RDS frequency deviation (Address: 26h - Write
 * Only)
 * @details RDSFDEV - RDS frequency deviation = 0.35KHz*RDSFDEV in normal mode.
 * RDS frequency deviation = 0.207KHz*RDSFDEV in 4k mode and private mode.
 * Values = from 0 to 127
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 34
 */

typedef union {
  struct {
    uint8_t RDSFDEV : 7; //!< RDS frequency deviation = 0.35KHz*RDSFDEV in normal mode. RDS frequency deviation = 0.207KHz*RDSFDEV in 4k mode and private mode. Values = from 0 to 127
    uint8_t line_in_en : 1; //!< Audio Line-in enable control. 0 = Disable; 1 = Enable
  } arg;
  uint8_t raw;
} qn8066_rds;

/**
 * @ingroup group00
 *
 * @brief GPLT - Transmitter soft chip threshold, gain of TX pilot (Address: 27h
 * - Write Only)
 *
 * | GAIN_TXPLT | value |
 * | ---------- | ----- |
 * |  7 - 0111  | 7% * 75KHz |
 * |  8 - 1000  | 8% * 75KHz |
 * |  9 - 1001  | 9% * 75KHz |
 * | 10 - 1010  | 10% * 75KHz |
 *
 * | t1m_sel    | value |
 * | ---------- | ----- |
 * |  0 - 00    | 57s |
 * |  1 - 01    | 58s |
 * |  2 - 10    | 59s |
 * |  3 - 11    | Infinity (Never) |
 *
 * | tx_sftclpth | value |
 * | ----------  | ----- |
 * |  0 - 00     | 12’d2051 (3db back off from 0.5v) |
 * |  1 - 01     | 12’d1725 (4.5db back off from 0.5v) |
 * |  2 - 10     | 12’d1452 (6db back off from 0.5v) |
 * |  3 - 11     | 12’d1028 (9db back off from 0.5v) |
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 34
 */

typedef union {
  struct {
    uint8_t GAIN_TXPLT : 4;   //!< Gain of TX pilot to adjust pilot frequency deviation. Refer to peak frequency deviation of MPX signal when audio input is full scale.
    uint8_t t1m_sel : 2;      //!< Selection of 1 minute time for PA off when no audio.
    uint8_t tx_sftclpth : 2;  //!< TX soft clip threshold
  } arg;
  uint8_t raw;
} qn8066_gplt;

/**
 * @ingroup group00
 *
 * @brief REG_VGA - X AGC gain (Address: 28h - Read and Write)
 * @details Attenuation/Gain depending on RIN - 0, 1, 2 AND 3 RESPECTIVELY. See tables below. 
 * | RIN        | Input impedance (K Ohoms) |
 * | ---------- | ------------------------- |
 * |  0 - 00    | 10    |
 * |  1 - 01    | 20    |
 * |  2 - 10    | 40    |
 * |  3 - 11    | 80    |
 *
 * | TXAGC_GDB  | TX digital gain |
 * | ---------- | ----- |
 * |  0 - 00    | 0 dB |
 * |  1 - 01    | 1 dB |
 * |  2 - 10    | 2 dB |
 * |  3 - 11    | Reserved |
 *
 * | TXAGC_GVGA  | Attenuation/Gain depending on RIN - 0, 1, 2 AND 3 RESPECTIVELY  |
 * | ----------  | ---------------------------------------------------------- |
 * |  0 - 000    |  3; -3; -9; -15   |
 * |  1 - 001    |  6;  0; -6; -12   |
 * |  2 - 010    |  9;  3; -3; -9    |
 * |  3 - 011    | 12;  6;  0; -6    |
 * |  4 - 100    | 15;  9;  3; -3    |
 * |  5 - 101    | 18; 12;  6;  0    |  
 * |  Others     | Reserved |
 * 
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable
 * Devices, pag. 35
 *
 */

typedef union {
  struct {
    uint8_t RIN : 2; //!< TX mode input impedance for both L/R channels. See
                     //!< table above
    uint8_t TXAGC_GDB : 2;   //!< TX digital gain. See table above.
    uint8_t TXAGC_GVGA : 3;  //!< TX input buffer gain. See table above
    uint8_t tx_sftclpen : 1; //!< TX soft clipping enable
  } arg;
  uint8_t raw;
} qn8066_reg_vga;


/**
 * @ingroup group00 RDS
 * @brief RDS - First block (RDS_BLOCK1 datatype)
 * @details PI Code Function: Identifies the radio station. This code is essential 
 * @details for allowing receivers to identify the source of the radio signal.
 */
typedef union { 
  struct {
    uint8_t reference : 7 ;
    uint8_t programId : 4 ;
    uint8_t countryId : 4 ;
  } field;
  unsigned char byteContent[2];
  uint16_t pi;   // pi Code
} RDS_BLOCK1;



/**
 * @ingroup group00 RDS
 *
 * @brief Block 2 (RDS_BLOCK2 data type)
 * @details Specifies the type of data being transmitted and includes information such as 
 * @details program type (e.g., news, music) and whether the station transmits traffic information.
 * @details The table below show some program types you can use  to check your transmitter.
 * | PTY Code	 | Program Type |
 * | ----------| ------------ |  
 * | 0	| No PTY (undefined)  | 
 * | 1	| News | 
 * | 3	| Information | 
 * | 4	| Sport | 
 * | 5	| Education | 
 * | 7	| Culture | 
 * | 8	| Science |
 * | 10	| Pop Music | 
 * | 11	| Rock Music |
 * | 15	| Other Music |
 * | 16	| Weather |
 * | 17	| Finance |
 * | 18	| Children's Programs |
 * | 20	| Religion |
 * | 24	| Jazz Music |
 * | 25	| Country Music |
 * | 26	| National Music |
 * | 27	| Oldies Music |
 * | 28	| Folk Music |
 * | 29	| Documentary |
 * | 31	| Alarm |
 *
 * @details For GCC on System-V ABI on 386-compatible (32-bit processors), the following stands:
 * 1) Bit-fields are allocated from right to left (least to most significant).
 * 2) A bit-field must entirely reside in a storage unit appropriate for its declared type.
 *    Thus a bit-field never crosses its unit boundary.
 * 3) Bit-fields may share a storage unit with other struct/union members, including members that are not bit-fields.
 *    Of course, struct members occupy different parts of the storage unit.
 * 4) Unnamed bit-fields' types do not affect the alignment of a structure or union, although individual
 *    bit-fields' member offsets obey the alignment constraints.
 * @see also https://en.wikipedia.org/wiki/Radio_Data_System
 *
 */

typedef union
{
    struct
    {
        uint16_t additionalData : 4;         //!< Additional data bits, depending on the group.
        uint16_t textABFlag : 1;             //!< Do something if it chanhes from binary "0" to binary "1" or vice-versa
        uint16_t programType : 5;            //!< PTY (Program Type) code
        uint16_t trafficProgramCode : 1;     //!< (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        uint16_t versionCode : 1;            //!< (B0) => 0=A; 1=B
        uint16_t groupType : 4;              //!< Group Type code.
    } commonFields;

    struct
    {
        uint16_t address : 2;                //!< Depends on Group Type and Version codes. If 0A or 0B it is the Text Segment Address.
        uint16_t DI : 1;                     //!< Decoder Control bit
        uint16_t MS : 1;                     //!< Music/Speech
        uint16_t TA : 1;                     //!< Traffic Announcement
        uint16_t programType : 5;            //!< PTY (Program Type) code
        uint16_t trafficProgramCode : 1;     //!< (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        uint16_t versionCode : 1;            //!< (B0) => 0=A; 1=B
        uint16_t groupType : 4;              //!< Group Type code.
    } group0Field;

    struct
    {
        uint16_t address : 4;                //!< Depends on Group Type and Version codes. If 2A or 2B it is the Text Segment Address.
        uint16_t textABFlag : 1;             //!< Do something if it changes from binary "0" to binary "1" or vice-versa
        uint16_t programType : 5;            //!< PTY (Program Type) code
        uint16_t trafficProgramCode : 1;     //!< (TP) => 0 = No Traffic Alerts; 1 = Station gives Traffic Alerts
        uint16_t versionCode : 1;            //!< (B0) => 0=A; 1=B
        uint16_t groupType : 4;              //!< Group Type code.
    } group2Field;
    unsigned char byteContent[2];
    uint16_t raw;                            //!< Raw 16-bit representation
} RDS_BLOCK2;

/**
 * @ingroup group00 RDS
 * @brief Block 3 (RDS_BLOCK3 data type)
 */
typedef union {
  unsigned char byteContent[2];
  uint16_t raw;
} RDS_BLOCK3;

/**
 * @ingroup group00 RDS
 * @brief Block 4 (RDS_BLOCK4 data type)
 */
typedef union {
  unsigned char byteContent[2];
  struct {
    uint16_t  offset: 5; //!< Local Time Offset  
    uint16_t  offset_sign: 1; //!< Offset sign (+/-) 
    uint16_t  min:  6; //!< UTC Minutes (0–59)  
    uint16_t  hour: 4; //!< Four least significant bits of the hour - UTC Hours (0–23)
  } utc;
  uint16_t raw;
} RDS_BLOCK4;

typedef union
{
    struct
    {
        uint32_t offset : 5;       //!< Local Time Offset
        uint32_t offset_sense : 1; //!< Local Offset Sign ( 0 = + , 1 = - )
        uint32_t minute : 6;       //!< UTC Minutes
        uint32_t hour : 5;         //!< UTC Hours
        uint32_t mjd : 17;         //!< Modified Julian Day Code
    } arg;
    uint8_t raw[6];
} RDS_DATE_TIME;


typedef union { 
  uint16_t value;
  uint8_t  raw[2];  
} WORD16;


/**
 * @ingroup  CLASSDEF
 * @brief QN8066 Class
 * @details This class implements all functions that will help you to control
 * the QN8066 devices.
 *
 * @author PU2CLR - Ricardo Lima Caratti
 */
class QN8066 {
private:
  uint16_t resetDelay = 1000;   //!<< Delay after reset (default 1s)
  uint16_t xtal_div = 1000;

  qn8066_system1 system1;
  qn8066_system2 system2;
  qn8066_gplt gplt;
  qn8066_cca cca;
  qn8066_fdev fdev;
  qn8066_int_ctrl int_ctrl;
  qn8066_txch txch;
  qn8066_xtal_div0 xtal_div0;
  qn8066_xtal_div1 xtal_div1;
  qn8066_xtal_div2 xtal_div2;
  qn8066_reg_vga reg_vga;
  qn8066_rds rds;
  qn8066_pac pac;
  qn8066_vol_ctl vol_ctl;


  uint8_t rdsSyncTime = 60;               //!< Wait time in milliseconds before sending the next group - Default is 60 ms. 
  uint8_t rdsRepeatGroup = 5;             //!< Number of times an RDS group will be transmitted consecutively.

  char rdsStationName[9] = " QN8066\r";   //!< Default Program Station (PS)
  uint16_t rdsPI = 33179;    //!< Default value for piCode (0x819B)
  uint8_t rdsPTY = 0;       //!< The default program type (PTY) is 5, which is "Education" for RDS and "Rock" for RDBS.
  uint8_t rdsTP = 0;        //!< Traffic Program (TP)
  uint8_t rdsSendError = 0;

  char strRxCurrentFrequency[8];  // Stores formated current frequency
  uint16_t rxCurrentFrequency; 
  uint8_t  rxCurrentStep = 1;     //!<  current frequency step. Default is 100kHz
  
  uint16_t minimalFrequency = 639;
  uint16_t maximalFrequency = 1081;



protected:
public:
  bool detectDevice();
  uint8_t scanI2CBus(uint8_t *device);

  uint8_t getRegister(uint8_t registerNumber);
  void setRegister(uint8_t registerNumber, uint8_t value);

  inline qn8066_cid1 getDeviceProductID() {
    qn8066_cid1 value;
    value.raw = this->getRegister(QN_CID1);
    return value;
  };

  inline qn8066_cid2 getDeviceProductFamily() {
    qn8066_cid2 value;
    value.raw = this->getRegister(QN_CID2);
    return value;
  }

  qn8066_status1 getStatus1();
  qn8066_status2 getStatus2();
  qn8066_status3 getStatus3();


  /**
   * @brief SYSTEM1 SETUP
   *
   */
  void setRX(uint16_t frequency);
  void setRxFrequency(uint16_t frequency);
  void setRxFrequencyUp();
  void setRxFrequencyDown();
  void setRxFrequencyStep(uint8_t value);
  inline uint16_t getRxCurrentFrequency(){ return this->rxCurrentFrequency;}; 
  void rdsEnableRX(bool value);
  void setAudioMuteRX(bool value);
  uint8_t getRxSNR();
  uint8_t getRxRSSI();
  void setRxFrequencyRange(uint16_t min = 640, uint16_t max = 1080);

  bool isValidRxChannel();
  bool isRxReceiving();
  bool isRxAgcStable(); 
  bool isRxStereo();
  void scanRxStation(uint16_t startFrequency, uint16_t stopFrequyency, uint8_t frequencyStep ); 

  // RX RDS
  void rdsRxEnable(bool value);
  char *rdsRxGetPS(char *ps);
  char *rdsRxGetRT(char *rt);
  char *rdsRxGetTime(char *time);
  
  
  
  void setTX(uint16_t frequency); // RESET the system and set to TX mode at a given frequency

  void  setTxStereo(bool value = true);  
  void  setTxMono(uint8_t value = 0); // Default stereo
  uint8_t getTxMono();
  void setTxPreEmphasis( uint8_t value = 75);
  void setPreEmphasis(uint8_t value = 1);

  void setTxOffAfterOneMinuteNoAudio(bool value);  
  void setTxOffAfterOneMinute(uint8_t value);
  void setTxPilotGain(uint8_t value);
  void setAudioAnalogGain(uint8_t value);
  void setAudioDigitalGain(uint8_t value);
  void setAudioDacHold(bool value);  
  void setAudioTxDiff(bool value);
  void setTxInputImpedance(uint8_t value);
  void setTxDigitalGain(uint8_t value);
  void setTxInputBufferGain(uint8_t value);
  void setTxSoftClippingEnable( bool value);
  void setTxSoftClipThreshold(uint8_t value);
  void setTxFrequencyDerivation(uint8_t value);

  void setResetDelay(uint16_t delayAfterReset) {
    this->resetDelay = delayAfterReset;
  };

  void begin();

  void setup(uint16_t xtalDiv = 1000, 
             bool mono = false, bool rds = false, uint8_t PreEmphasis = 0, 
             uint8_t xtalInj = 0, uint8_t imageRejection = 1, 
             uint8_t txSoftClipThreshold = 0,  uint8_t oneMinutOff = 3, uint8_t gainTxPLT =9,
             uint8_t txFreqDev = 125, uint8_t rdsLineIn = 0, uint8_t rdsFreqDev = 60, 
             uint8_t inImpedance = 1, uint8_t txAgcDig = 0, uint8_t txAgcBuffer = 1 , uint8_t txSoftClip = 0 );


  void updateTxSetup();


  void setTxMode(uint8_t value);
  void stopTransmitting();
  void startTransmitting();


  void setXtal(uint16_t divider, uint8_t xtalInj, uint8_t imageRejection);

  void setPAC(uint8_t PA_TRGT);
  
  void setToggleTxPdClear();

  /**
   * @ingroup group04 PA Control
   * @brief TX Audio peak clear signal.
   * @details Same setToggleTxPdClear (synonym)
   * @details Audio peak value is max-hold and stored in aud_pk (see STATUS register). Once TXPD_CLR is toggled, the aud_pk value is cleared and restarted again
   * @details Example
   * @code 
   * #include <QN8066.h>
   * QN8066 tx;
   * void setup() {
   *   tx.setup();
   *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
   *   ...
   *   tx.resetAudioPeak();   
   * }
   * void loop() {
   * }
   * @endcode  
   */
  inline void resetAudioPeak() {this->setToggleTxPdClear();};
  int  getAudioPeakValue();


  void setStnby(bool value);


  // RDS TX

  void rdsSetMode(uint8_t mode); 
  void rdsSet4KMode(uint8_t value);
  void rdsInitTx(uint8_t countryId = 0, uint8_t programId = 0, uint8_t reference = 0, uint8_t pty = 1, uint8_t rdsSyncTime = 60, uint8_t rdsRepeatGroup = 5 );
  void rdsSetInterrupt(uint8_t value);

  void rdsTxEnable(bool value);   
  uint8_t rdsSetTxToggle();
  bool rdsGetTxUpdated();
  void rdsSetFrequencyDerivation(uint8_t freq = 6);
  void rdsSetTxLineIn(bool value = 0); 

  void rdsSendGroup(RDS_BLOCK1 blockA, RDS_BLOCK2 blockB, RDS_BLOCK3 blockC, RDS_BLOCK4 blockD);
  void rdsSendPS(char* ps = NULL); 


  void rdsSetStationName(char *stationName);
  void rdsSendRTMessage(char *rt);
  /**
 * @ingroup group05 TX RDS
 * @brief Sends RDS Radio Text Message (group 2A)
 * @details It is a synonym to rdsSendRTMessage. This function repeats sending a group this->rdsRepeatGroup times.
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
 *   tx.rdsSendRT("IT IS AN EXAMPLE..."); // transmits the message "IT IS AN EXAMPLE..." this->rdsRepeatGroup times
 * }
 *
 * void loop() {
 * }
 * @endcode  
 */
  inline void rdsSendRT(char *rt) {this->rdsSendRTMessage(rt); };

  int32_t calculateMJD(uint16_t year, uint8_t month, uint8_t day);
  void rdsSendDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, int8_t offset);


  
 /**
  * @ingroup group05 TX RDS
  * @brief Sets the Program Identification (PI)
  * @details The Program Identification (PI) code is a critical component of the RDS (Radio Data System) protocol. 
  * @details It is used to uniquely identify the radio station transmitting the RDS data. The PI code is a 16-bit 
  * @details value (4 hexadecimal digits) assigned to each station.
  * @details PI Code Structure
  * @details The PI code is a 16-bit binary value (4 hexadecimal digits), and its structure is as follows:
  * @details a)	Country Identifier (First 4 Bits) - The first 4 bits of the PI code represent the country identifier. 
  * @details    This helps to differentiate stations in different countries. For example, the code 0x1 might represent 
  * @details    one country, while 0x2 represents another. 
  * @details b)	Program Type (Next 4 Bits) - The next 4 bits indicate the program type. This categorizes the type of content 
  * @details    being broadcast, such as music, news, sports, etc.
  * @details c) Program Reference Number (Last 8 Bits) - The last 8 bits provide a unique reference number for the specific 
  * @details    station or program.
  * @details You can set the PI code using the rdsSetPI. The are three ways of setting PI code: 
  * @details rdsInitTx(countryId, programId, reference); rdsSetPI(countryId, programId, reference);
  * @details rdsSetPI(piCode);
  * @see rdsInitTx
  * @details Example
  * @code 
  * #include <QN8066.h>
  * QN8066 tx;
  * 
  * uint8_t countryId = 0x1;
  * uint8_t programId = 0x2;
  * uint8_t reference = 0x34;
  * 
  * void setup() {
  *   tx.setup();
  *   tx.setTX(1069); // Set the transmitter to 106.9 MHz 
  *   ...
  *   tx.rdsInitTx(countryId, programId, reference); // You can use this
  *   ...  
  *   tx.rdsSetPI(countryId, programId, reference);  // Or this
  *   ... 
  *   tx.rdsSetPI(0x1234);                           // Or this
  * }
  *
  * void loop() {
  * }
  * @endcode     
  * @param pi - PI Code
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup 
  */
  void rdsSetPI(uint16_t pi) {this->rdsPI = pi;};

  void rdsSetPI(uint8_t countryId, uint8_t programId, uint8_t reference = 0) ;


  /**
  * @ingroup group05 TX RDS
  * @brief Gets the Program Identification (PI)
  * @param pi - PI Code
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  uint8_t rdsGetPI() {return this->rdsPI;};

  /**
  * @ingroup group05 TX RDS
  * @brief Sets the Program Type (PTY)
  * @param pty - Program type
  * @see The table of PTY Program Type for RDS and RDBS can be checkd here: https://en.wikipedia.org/wiki/Radio_Data_System
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  void rdsSetPTY(uint16_t pty) {this->rdsPTY = pty;};

  /**
  * @ingroup group05 TX RDS
  * @brief Gets the Program Type (PTY)
  * @param pty - Program type
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  uint8_t rdsGetPTY() {return this->rdsPTY;};


  /**
  * @ingroup group05 TX RDS
  * @brief Sets the Traffic Program.
  * @param tp - tp Code
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  void rdsSetTP(uint16_t tp) {this->rdsTP = tp;};

  /**
  * @ingroup group05 TX RDS
  * @brief Sets the Traffic Program.
  * @param tp - tp Code
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup  
  */
  uint8_t rdsGetTP() {return this->rdsTP;};


  /**
  * @ingroup group05 TX RDS
  * @brief Gets the Program Station (Station Name).
  */
  char* rdsGetPS() {return this->rdsStationName;};


  /**
  * @ingroup group05 TX RDS
  * @brief Gets TX RDS error setup
  */
  uint8_t rdsGetError() {return this->rdsSendError;};

  /**
   * @ingroup group05 TX RDS
   * @brief Clear RDS register (Buffer)
   */
  void rdsClearBuffer();


  /**
  * @ingroup group05 TX RDS
  * @brief Sets the wait time for the QN8066 to be available to send the next RDS block.
  * @details The default time is 60ms, but depending on the microcontroller you are using, it may be necessary to reduce this time.
  * @param syncTime - time in ms 
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  inline void rdsSetSyncTime(uint8_t syncTime) {this->rdsSyncTime = syncTime; };
  
    /**
  * @ingroup group05 TX RDS
  * @brief Sets the number of time that a group will be sent at once 
  * @details The default value is 5. 
  * @param count - number of times 
  * @see rdsSetPI, rdsInitTx, rdsTxEnable, rdsGetPI, rdsSetPTY, rdsGetPTY, rdsSetTP, rdsGetTP rdsSetSyncTime, rdsSetRepeatSendGroup   
  */
  inline void rdsSetRepeatSendGroup (uint8_t count) {this->rdsRepeatGroup = count;};

  void resetFsm();
  uint8_t getFsmStateCode();

  void convertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator = '.', bool remove_leading_zeros = true);
  char* formatCurrentFrequency(char decimalSeparator = ',');

/*******************************************************************************
  * The functions below modify the clock frequency for I2C communication.
  * 100kHz  is usually the baseline.
  * Use one of these funcition if you have a problem on the default configuration.
  *******************************************************************************/

  /**
   * @ingroup group99 MCU I2C Speed
   * @brief Sets I2C bus to 10kHz
   */
   inline void setI2CLowSpeedMode(void)
  {
       Wire.setClock(10000);
  };

    /**
     * @ingroup group99 MCU I2C Speed
     *
     * @brief Sets I2C bus to 100kHz
     */
    inline void setI2CStandardMode(void) { Wire.setClock(100000); };

    /**
     * @ingroup group99 MCU I2C Speed
     *
     * @brief Sets I2C bus to 400kHz
     */
    inline void setI2CFastMode(void)
    {
        Wire.setClock(400000);
    };

    /**
     * @ingroup group99 MCU I2C Speed
     *
     * @brief Sets the I2C bus to a given value.
     * ATTENTION: use this function with caution
     *
     * @param value in Hz. For example: The values 500000 sets the bus to 500kHz.
     */
    inline void setI2CFastModeCustom(long value = 500000) { Wire.setClock(value); };



};
#endif // _QN8066_H