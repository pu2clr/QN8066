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
 * @brief Sets device modes.
 *
 * @see 
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

} qn8066_system1;




#endif // _QN8066_H