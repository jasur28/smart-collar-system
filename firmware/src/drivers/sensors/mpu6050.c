#include "mpu6050.h"

#include <avr/io.h>

/* ===== MPU6050 I2C address ===== */
#define MPU6050_ADDR_W  0xD0
#define MPU6050_ADDR_R  0xD1

/* ===== Low-level I2C (TWI) ===== */

static void i2c_init(void) {
    TWSR = 0x00;      // prescaler = 1
    TWBR = 72;        // ~100kHz @ 16MHz
}

static void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

static void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

static void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

static uint8_t i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

/* ===== MPU6050 helpers ===== */

static int16_t mpu6050_read_word(uint8_t reg) {
    i2c_start();
    i2c_write(MPU6050_ADDR_W);
    i2c_write(reg);

    i2c_start();                  // repeated START
    i2c_write(MPU6050_ADDR_R);

    uint8_t hi = i2c_read_nack();
    uint8_t lo = i2c_read_nack();

    i2c_stop();
    return (int16_t)((hi << 8) | lo);
}

/* ===== Public API ===== */

void mpu6050_init(void) {
    i2c_init();

    i2c_start();
    i2c_write(MPU6050_ADDR_W);
    i2c_write(0x6B);   // PWR_MGMT_1
    i2c_write(0x00);   // wake up
    i2c_stop();
}

void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    if (!ax || !ay || !az) {
        return;
    }

    *ax = mpu6050_read_word(0x3B); // ACCEL_XOUT_H
    *ay = mpu6050_read_word(0x3D); // ACCEL_YOUT_H
    *az = mpu6050_read_word(0x3F); // ACCEL_ZOUT_H
}
