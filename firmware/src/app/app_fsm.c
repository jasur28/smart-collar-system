#include "app_fsm.h"

#include "drivers/sensors/ds18b20.h"
#include "drivers/sensors/mpu6050.h"
#include "drivers/comm/uart.h"
#include "drivers/power/sleep.h"

#include <stdint.h>
#include <avr/interrupt.h>

/* ===== FSM internal state ===== */

static app_state_t current_state;

/* Cached sensor data */
static int16_t temp_raw;
static int16_t acc_x;
static int16_t acc_y;
static int16_t acc_z;

/* ===== public API ===== */

void app_fsm_init(void) {
    current_state = APP_STATE_INIT;
}

void app_fsm_step(void) {
    switch (current_state) {

    case APP_STATE_INIT:
        /* Initialize application-level modules */
        ds18b20_init();
        mpu6050_init();
        sleep_init();     // initialize watchdog sleep

        current_state = APP_STATE_READ_SENSORS;
        break;

    case APP_STATE_READ_SENSORS:
         /* Read sensors */
        temp_raw = ds18b20_read_raw();
        mpu6050_read_accel(&acc_x, &acc_y, &acc_z);

        current_state = APP_STATE_SEND_UART;
        break;

    case APP_STATE_SEND_UART:
        /* Send data over UART */
        cli();
        uart_print("T=");
        uart_print_int(temp_raw / 16);
        uart_print("C ");

        uart_print("AX=");
        uart_print_int(acc_x);
        uart_print(" AY=");
        uart_print_int(acc_y);
        uart_print(" AZ=");
        uart_print_int(acc_z);

        uart_print("\r\n");

        /* Wait until UART transmission is fully completed */
        uart_flush();
        
        sei();
        current_state = APP_STATE_SLEEP;
        break;

    case APP_STATE_SLEEP:
        /* Enter low power sleep, wakeup by watchdog */
        sleep_enter();

        current_state = APP_STATE_READ_SENSORS;
        break;

    default:
        current_state = APP_STATE_INIT;
        break;
    }
}
