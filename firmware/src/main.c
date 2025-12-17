#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "drivers/sensors/ds18b20.h"
/* ================= UART (USB-TTL) ================= */

void uart_init(void) {
    uint16_t ubrr = F_CPU / 16 / 9600 - 1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char *s) {
    while (*s) uart_tx(*s++);
}

void uart_print_int(int16_t v) {
    char buf[7];
    uint8_t i = 0;
    if (v < 0) {
        uart_tx('-');
        v = -v;
    }
    do {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    } while (v);
    while (i--) uart_tx(buf[i]);
}

/* ================= I2C (MPU6050) ================= */

void i2c_init(void) {
    TWSR = 0x00;
    TWBR = 72; // ~100kHz
}

void i2c_start(void) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

void i2c_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

uint8_t i2c_read_nack(void) {
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

int16_t mpu_read_word(uint8_t reg) {
    i2c_start();
    i2c_write(0xD0); // MPU write
    i2c_write(reg);
    i2c_start();
    i2c_write(0xD1); // MPU read
    uint8_t hi = i2c_read_nack();
    uint8_t lo = i2c_read_nack();
    i2c_stop();
    return (hi << 8) | lo;
}

void mpu_init(void) {
    i2c_start();
    i2c_write(0xD0);
    i2c_write(0x6B);
    i2c_write(0x00); // wake up
    i2c_stop();
}



/* ================= MAIN ================= */

int main(void) {
    uart_init();
    i2c_init();
    mpu_init();
    ds18b20_init();

 

    _delay_ms(2000);   // <<< ВАЖНО
    uart_print("SMART COLLAR | USB-TTL | AVR\n");

    while (1) {
        int16_t temp = ds18b20_read_raw();
        int16_t ax = mpu_read_word(0x3B);
        int16_t ay = mpu_read_word(0x3D);
        int16_t az = mpu_read_word(0x3F);

        uart_print("T=");
        uart_print_int(temp / 16);
        uart_print("C ");

        uart_print("AX=");
        uart_print_int(ax);
        uart_print(" AY=");
        uart_print_int(ay);
        uart_print(" AZ=");
        uart_print_int(az);
        uart_print("\r\n");

        _delay_ms(2000);
    }
}

