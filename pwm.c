#include "pwm.h"
#include <avr/io.h>

static volatile uint16_t ail2_ticks = 3000;
static volatile uint16_t rudder_ticks = 3000;

void pwm_init(void) {
    // Piny pod PWM sprzętowy: PB1 (9), PB2 (10)
    // Piny pod PWM programowy: PB3 (11), PD6 (6)
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3);
    DDRD |= (1 << PD6);

    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    ICR1 = 39999; 
    OCR1A = 3000; 
    OCR1B = 3000;
}

void pwm_set_hardware(uint16_t ch1_ticks, uint16_t ch2_ticks) {
    OCR1A = ch1_ticks;
    OCR1B = ch2_ticks;
}

void pwm_set_software(uint16_t ail_t, uint16_t rud_t) {
    ail2_ticks = ail_t;
    rudder_ticks = rud_t;
}

// Funkcja odpowiedzialna za przełączanie pinów 11 i 6
void pwm_process_software(void) {
    uint16_t current_ticks = TCNT1;

    if (current_ticks < ail2_ticks) {
        PORTB |= (1 << PB3);
    } else {
        PORTB &= ~(1 << PB3);
    }

    if (current_ticks < rudder_ticks) {
        PORTD |= (1 << PD6);
    } else {
        PORTD &= ~(1 << PD6);
    }
}