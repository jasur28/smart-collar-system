#pragma once

#include <stdint.h>

/* Состояния приложения */
typedef enum {
    APP_STATE_INIT = 0,
    APP_STATE_READ_SENSORS,
    APP_STATE_SEND_UART,
    APP_STATE_SLEEP
} app_state_t;

/* Инициализация FSM */
void app_fsm_init(void);

/* Один шаг FSM (вызывается в main loop) */
void app_fsm_step(void);
