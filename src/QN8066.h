/**
 * @brief QN8066 ARDUINO LIBRARY
 *
 *
 * @author PU2CLR - Ricardo Lima Caratti
 * @date  2024-06
 */

#ifndef _QN8066_H // Prevent this file from being compiled more than once
#define _QN8066_H

#include <Arduino.h>
#include <Wire.h>


// ATENÇÃO/LEMBRETE -->> ERRO - INVERTER AS ORDENS DOS BITS DAS ESTRUTURAS ABAIXO

/** @defgroup group01 Union, Struct and Defined Data Types
 * @section group01 Data Types
 *
 * @brief QN8066 data representation
 *
 * @details 
 * 
 */

/**
 * @ingroup group01
 *
 * @brief System1 - Sets device modes (Address: 00h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 19
 */

typedef union
{
    struct
    {
        uint8_t swrst : 1;          //!< Reset all registers to default values; 0 = Keep the current value; 1 = Reset to default values
        uint8_t recal : 1;          //!< Reset the state to initial states and recalibrate all blocks - 0 = No action. FSM runs normally;  1 =  Reset the FSM. After this bit is de-asserted, FSM will go through all the power up and calibration sequence.
        uint8_t stnby : 1;          //!< Request Immediately enter Standby mode whatever state chip is in - 0 = Non standby mode; 1 = Enter standby mode
        uint8_t rxreq : 1;          //!< Receiving request - 0 = Non RX mode; 1 = Enter Receiving mode 
        uint8_t txreq : 1;          //!< Transmission request - 0 = Non TX mode; 1 = Enter transmit mode 
        uint8_t chsc : 1;           //!< Channel Scan mode enable - 0 = Normal operation; 1 = Channel Scan mode operation
        uint8_t ccs_ch_dis : 1;     //!<  0 = TX_CH is decided by internal CCS; 1 = TX_CH is decided writing in TX_CH[9:0]
        uint8_t cca_ch_dis : 1;     //!<  0 = RX_CH is decided by internal CCA; 1 = RX_CH is decided writing in RX_CH[9:0]
    } arg;   
    uint8_t raw;           
} qn8066_system1;


/**
 * @ingroup group01
 *
 * @brief System2 - Sets device modes (Address: 01h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 20
 */

typedef union
{
    struct
    {
        uint8_t rx_rdsen : 1;  //!<  Receiver RDS enable; 0 = RDS Disable; 1 = RDS Enable
        uint8_t tx_rdsen : 1;  //!<  Transmitter RDS enable; 0 = RDS Disable; 1 = RDS Enable
        uint8_t force_mo : 1;  //!<  Force receiver in MONO mode; 0 = Not forced. ST/MONO auto selected; Forced in MONO mode
        uint8_t tx_mono : 1;   //!<  TX stereo and mono mode selection; 0 = Stereo;  1 = Mono
        uint8_t rx_mute : 1;   //!<  RX audio Mute enable - 0 = Mute Disabled; 1 = Mute Enabled
        uint8_t tx_mute : 1;   //!<  TX audio mute enabel - 0 = Mute Disabled; 1 = Mute Enabled
        uint8_t rdsrdy : 1;    //!<  RDS transmitting ready; - If user want the chip transmitting all the 8 bytes in RDS0~RDS7, user should toggle this bit.
        uint8_t tc : 1;        //!<  Pre-emphasis and de-emphasis time constant; 0 = 50; 1 = 75
    } arg;              
    uint8_t raw;   
} qn8066_system2;


/**
 * @ingroup group01
 *
 * @brief CCA - Sets CCA parameters ( Address: 02h)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 21
 */


typedef union
{
    struct
    {
        uint8_t xtal_inj : 1;    //!<  Select the reference clock source. 0 = Inject sine-wave clock; 1 = Inject digital clock
        uint8_t imr : 1;         //!<  Image Rejection. 0 = LO<RF, image is in lower side; 1 = LO>RF, image is in upper side
        uint8_t SNR_CCA_TH : 6;  //!<  The threshold for determination of whether current channel is valid by check its SNR.  
    } arg;              
    uint8_t raw;   
} qn8066_cca;


/**
 * @ingroup group01
 *
 * @brief SRN - Estimate RF input CNR value( Address: 03h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 21
 */

typedef union
{
    uint8_t SNRDB;           
    uint8_t raw;   
} qn8066_srn;


/**
 * @ingroup group01
 *
 * @brief RSSISIG - In-band signal RSSI (Received signal strength indicator) dBuV value. dBuV=RSSI-49( Address: 04h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 22
 */

typedef union
{
    uint8_t RSSISIG;           
    uint8_t raw;   
} qn8066_rssisig;



/**
 * @ingroup group01
 *
 * @brief CID1 - Device ID numbers ( Address: 05h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 22
 */


typedef union
{
    uint8_t CID1;
    uint8_t raw;   
} qn8066_cid1;


/**
 * @ingroup group01
 *
 * @brief CID2 - Device ID numbers ( Address: 06h - Read Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 22
 */

typedef union
{
    struct
    {
        uint8_t CID3 : 6;   //!<  Chip ID for product ID. 001101 = Transceiver – QN8066; Others = Reserved unkown 
        uint8_t CID4 : 2;   //!<  Sequency integer values from 0 to 4.
    } arg;   
    uint8_t raw;   
} qn8066_cid2;

/**
 * @ingroup group01
 *
 * @brief XTAL_DIV0 - Frequency select of reference clock source (Lower bits -  Address: 07h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 23
 */

typedef union
{
    uint8_t xtal_div;       // !< Lower 8 bits of xtal_div[10:0]. Xtal_div[10:0] = round(freq of xtal/32.768KHz).
    uint8_t raw;   
} qn8066_xtal_div0;


/**
 * @ingroup group01
 *
 * @brief XTAL_DIV1 - Frequency select of reference clock source (Lower bits -  Address: 07h - Write Only)
 *
 * @see Data Sheet - Quintic - QN8066 - Digital FM Transceiver for Portable Devices, pag. 23
 */

typedef union
{
    struct
    {
        uint8_t pll_dlt  : 5;   //!<  Lower 5 bits of pll_dlt[12:0].
        uint8_t xtal_div : 3;   //!<  Higher 3 bits of xtal_div[10:0]. Xtal_div[10:0] = round(freq of xtal/32.768KHz)
    } arg;  
    uint8_t raw;   
} qn8066_xtal_div1;




/*
// TEMPLATE
typedef union
{
    struct
    {
        uint8_t a : 1;          //!< 
        uint8_t b : 1;          //!< 
        uint8_t c : 1;          //!< 
        uint8_t d : 1;          //!<  
        uint8_t e : 1;          //!< 
        uint8_t f : 1;          //!< 
        uint8_t g : 1;          //!<  
        uint8_t h : 1;          //!<  
    } arg;              
    uint8_t raw;   
} qn8066_xx;
*/



#endif // _QN8066_H