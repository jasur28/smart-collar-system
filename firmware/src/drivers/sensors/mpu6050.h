#pragma once

#include <stdint.h>

/**
 * @brief Инициализация MPU6050
 * Вывод из sleep
 */
void mpu6050_init(void);

/**
 * @brief Чтение ускорений (RAW)
 * @param ax, ay, az — указатели на int16_t
 */
void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az);
