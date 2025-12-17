#include "sleep.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

/* Watchdog interrupt only */
ISR(WDT_vect) {
    /* Wake-up interrupt, nothing to do here */
}

void sleep_init(void) {
    cli();

    /* Reset watchdog */
    wdt_reset();

    /* Enable watchdog configuration */
    WDTCSR = (1 << WDCE) | (1 << WDE);

    /* 
     * Watchdog interrupt mode
     * Timeout ~2 seconds (WDP2 | WDP1)
     */
    WDTCSR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1);

    sei();
}

void sleep_enter(void) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}
