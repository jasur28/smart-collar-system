#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

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

/* ================= DS18B20 (OneWire) ================= */

#define OW_DDR  DDRD
#define OW_PORT PORTD
#define OW_PIN  PIND
#define OW_BIT  PD2

void ow_low(void) {
    OW_DDR |= (1<<OW_BIT);
    OW_PORT &= ~(1<<OW_BIT);
}

void ow_release(void) {
    OW_DDR &= ~(1<<OW_BIT);
}

uint8_t ow_reset(void) {
    ow_low();
    _delay_us(480);
    ow_release();
    _delay_us(70);
    uint8_t ok = !(OW_PIN & (1<<OW_BIT));
    _delay_us(410);
    return ok;
}

// void ow_write_bit(uint8_t b) {
//     ow_low();
//     if (b) _delay_us(6);
//     else _delay_us(60);
//     ow_release();
//     _delay_us(10);
// }
void ow_write_bit(uint8_t b) {
    ow_low();
    if (b) {
        _delay_us(6);    // write '1'
        ow_release();
        _delay_us(64);
    } else {
        _delay_us(60);   // write '0'
        ow_release();
        _delay_us(10);
    }
}


// uint8_t ow_read_bit(void) {
//     ow_low();
//     _delay_us(6);
//     ow_release();
//     _delay_us(9);
//     uint8_t b = (OW_PIN & (1<<OW_BIT)) != 0;
//     _delay_us(55);
//     return b;
// }
uint8_t ow_read_bit(void) {
    uint8_t bit;

    ow_low();            // master pulls low
    _delay_us(3);        // ≥1µs
    ow_release();        // release bus
    _delay_us(12);       // ❗ ЖДЁМ, пока датчик выставит бит

    bit = (OW_PIN & (1 << OW_BIT)) ? 1 : 0;

    _delay_us(50);       // конец тайм-слота (~60µs total)
    return bit;
}


void ow_write_byte(uint8_t b) {
    for (uint8_t i=0;i<8;i++) {
        ow_write_bit(b & 1);
        b >>= 1;
    }
}

// uint8_t ow_read_byte(void) {
//     uint8_t v = 0;
//     for (uint8_t i=0;i<8;i++) {
//         v >>= 1;
//         if (ow_read_bit()) v |= 0x80;
//     }
//     return v;
// }
uint8_t ow_read_byte(void) {
    uint8_t v = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (ow_read_bit()) {
            v |= (1 << i);   // LSB → MSB
        }
    }
    return v;
}


int16_t ds18b20_read(void) {
    if (!ow_reset()) return 0x7FFF;   // нет датчика

    ow_write_byte(0xCC); // SKIP ROM
    ow_write_byte(0x44); // CONVERT T

    // Ждём, пока датчик закончит (он держит линию низкой)
    while (!ow_read_bit());

    if (!ow_reset()) return 0x7FFF;

    ow_write_byte(0xCC); // SKIP ROM
    ow_write_byte(0xBE); // READ SCRATCHPAD

    uint8_t lo = ow_read_byte();
    uint8_t hi = ow_read_byte();

    return (hi << 8) | lo;
}

/* ================= MAIN ================= */

int main(void) {
    uart_init();
    i2c_init();
    mpu_init();

 

    _delay_ms(2000);   // <<< ВАЖНО
    uart_print("SMART COLLAR | USB-TTL | AVR\n");

    while (1) {
        
        int16_t temp = ds18b20_read();
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

