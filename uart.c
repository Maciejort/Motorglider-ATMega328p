#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Rzeczywista deklaracja zmiennych
volatile uint16_t rc_channels[14];
volatile uint8_t frame_ready = 0;

void uart_init(void) {
    UCSR0A = (1 << U2X0);
    UBRR0H = 0;
    UBRR0L = 16;
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
}

ISR(USART_RX_vect) {
    static uint8_t buffer[32];
    static uint8_t index = 0;
    uint8_t data = UDR0;
    int i;

    if (index == 0 && data != 0x20) return;
    if (index == 1 && data != 0x40) { index = 0; return; }

    buffer[index++] = data;

    if (index == 32) {
        index = 0;
        uint16_t checksum = 0xFFFF;
        
        for (i = 0; i < 30; i++) {
            checksum -= buffer[i];
        }
        
        uint16_t rx_checksum = buffer[30] | (buffer[31] << 8);

        if (checksum == rx_checksum) {
            for (i = 0; i < 14; i++) {
                rc_channels[i] = buffer[2 + i*2] | (buffer[3 + i*2] << 8);
            }
            frame_ready = 1;
        }
    }
}