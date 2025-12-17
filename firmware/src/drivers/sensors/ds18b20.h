#pragma once

#include <stdint.h>

/**
* @brief Initialize DS18B20
* 
*/
void ds18b20_init(void);

/**
 * @brief Read temperature (RAW)
 * @return int16_t
 *         value * 1/16 °C
 *         0x7FFF — error (sensor not found)
 */
int16_t ds18b20_read_raw(void);
