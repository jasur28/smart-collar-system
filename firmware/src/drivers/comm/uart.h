#pragma once

#include <stdint.h>

/**
 * @brief Initialize UART (TX only)
 */
void uart_init(void);

/**
 * @brief Send single character over UART
 */
void uart_tx(char c);

/**
 * @brief Send null-terminated string over UART
 */
void uart_print(const char *s);

/**
 * @brief Send signed 16-bit integer as decimal string
 */
void uart_print_int(int16_t value);
