#include "app_fsm.h"

#include "../drivers/sensors/ds18b20.h"
#include "drivers/sensors/mpu6050.h"

#include <util/delay.h>
#include <stdint.h>

/* ===== внутреннее состояние FSM ===== */

static app_state_t current_state;

/* данные датчиков (static → живут между вызовами) */
static int16_t temp_raw;
static int16_t acc_x;
static int16_t acc_y;
static int16_t acc_z;

/* ===== public API ===== */

void app_fsm_init(void) {
    current_state = APP_STATE_INIT;
}

/* один шаг FSM */
void app_fsm_step(void) {
    switch (current_state) {

    case APP_STATE_INIT:
        /* здесь только инициализация логики */
        ds18b20_init();
        mpu6050_init();

        current_state = APP_STATE_READ_SENSORS;
        break;

    case APP_STATE_READ_SENSORS:
        temp_raw = ds18b20_read_raw();
        mpu6050_read_accel(&acc_x, &acc_y, &acc_z);

        current_state = APP_STATE_SEND_UART;
        break;

    case APP_STATE_SEND_UART:
        /* ⚠️ пока UART остаётся в main через функцию */
        /* здесь только подготовка данных */

        current_state = APP_STATE_SLEEP;
        break;

    case APP_STATE_SLEEP:
        /* пока просто задержка, позже будет sleep_cpu() */
        _delay_ms(2000);

        current_state = APP_STATE_READ_SENSORS;
        break;

    default:
        current_state = APP_STATE_INIT;
        break;
    }
}
