#include "uart.h"

#include <avr/io.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

void uart_init(void) {
    uint16_t ubrr = (F_CPU / 16 / 9600) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    UCSR0B = (1 << TXEN0);   // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char *s) {
    while (*s) {
        uart_tx(*s++);
    }
}

void uart_print_int(int16_t value) {
    char buf[7];
    uint8_t i = 0;

    if (value < 0) {
        uart_tx('-');
        value = -value;
    }

    do {
        buf[i++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value);

    while (i--) {
        uart_tx(buf[i]);
    }
}
