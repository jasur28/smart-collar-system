#include <avr/io.h>

#include "app/app_fsm.h"
#include "drivers/comm/uart.h"   // пока UART остаётся здесь

int main(void) {
    uart_init();

    app_fsm_init();

    while (1) {
        app_fsm_step();
    }
}
