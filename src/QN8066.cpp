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
    return  Wire.endTransmission();

}
