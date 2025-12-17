#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "drivers/sensors/ds18b20.h"
#include "drivers/sensors/mpu6050.h"
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



/* ================= MAIN ================= */

int main(void) {
    uart_init();
    mpu6050_init();
    ds18b20_init();

 

    _delay_ms(2000);   // <<< ВАЖНО
    uart_print("SMART COLLAR | USB-TTL | AVR\n");

    while (1) {
        int16_t temp = ds18b20_read_raw();
        int16_t ax, ay, az;
        mpu6050_read_accel(&ax, &ay, &az);

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

