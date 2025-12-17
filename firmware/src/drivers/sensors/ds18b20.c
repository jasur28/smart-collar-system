#include "ds18b20.h"

#include <avr/io.h>
#include <util/delay.h>

/* ========== OneWire pin configuration ===========*/

#define OW_DDR  DDRD
#define OW_PORT PORTD
#define OW_PIN  PIND
#define OW_BIT  PD2


/* ========= Low-level OneWire primitives =========*/

static void ow_low(void)
{
    OW_DDR |= (1 << OW_BIT);
    OW_PORT &= (1 << OW_BIT);
}

static void ow_release(void)
{
    OW_DDR &= ~(1 << OW_BIT);
}

static uint8_t ow_reset(void)
{
    ow_low();
    _delay_us(480);
    ow_release();
    _delay_us(70);
    uint8_t ok = !(OW_PIN & (1 << OW_BIT));
    _delay_us(410);
    return ok;
}

static void ow_write_bit(uint8_t b) {
    ow_low();
    if (b) {
        _delay_us(6);
        ow_release();
        _delay_us(64);
    } else {
        _delay_us(60);
        ow_release();
        _delay_us(10);
    }
}

static uint8_t ow_read_bit(void) {
    uint8_t bit;

    ow_low();
    _delay_us(3);
    ow_release();
    _delay_us(12);

    bit = (OW_PIN & (1 << OW_BIT)) ? 1 : 0;

    _delay_us(50);
    return bit;
}

static void ow_write_byte(uint8_t b)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        ow_write_bit(b & 1);
        b >>= 1;
    }
}

static uint8_t ow_read_byte(void)
{
    uint8_t v = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        if(ow_read_bit())
        {
            v |= (1 << i); // LSB first
        }
    }

    return v;
}

/* ===== Public DS18B20 API =====*/

void ds18b20_init(void)
{

}

int16_t ds18b20_read_raw(void)
{
    if(!ow_reset())
        return 0x7FFF;

    ow_write_byte(0xCC); //SKIP ROM
    ow_write_byte(0x44); //CONVERT T

    //wait conversation
    while(!ow_read_bit());

    if(!ow_reset())
        return 0x7FFF;

    ow_write_byte(0xCC); //SKIP ROM
    ow_write_byte(0xBE); //READ SCRATCHPAD

    uint8_t lo = ow_read_byte();
    uint8_t hi = ow_read_byte();

    return (uint16_t)((hi << 8) | lo);
}