#include <avr/interrupt.h>
#include "uart.h"
#include "pwm.h"

int main(void) {
    uart_init();
    pwm_init();
    sei();

    while(1) {
        if (frame_ready) {
            frame_ready = 0;

            // Zbieranie i przeliczanie danych
            uint16_t aileron_1_ticks = rc_channels[0] * 2;
            uint16_t elevator_ticks  = rc_channels[1] * 2;
            uint16_t aileron_2_ticks = (3000 - rc_channels[0]) * 2;
            uint16_t rudder_ticks    = rc_channels[3] * 2;

            // Wysyłanie do modułu PWM
            pwm_set_hardware(aileron_1_ticks, elevator_ticks);
            pwm_set_software(aileron_2_ticks, rudder_ticks);
        }

        // Utrzymywanie programowego PWM przy życiu w pętli
        pwm_process_software();
    }
    
    return 0;
}