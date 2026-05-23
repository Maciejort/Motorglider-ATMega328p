#ifndef UART_H
#define UART_H

#include <stdint.h>

// Udostępniamy zmienne dla innych plików
extern volatile uint16_t rc_channels[14];
extern volatile uint8_t frame_ready;

void uart_init(void);

#endif