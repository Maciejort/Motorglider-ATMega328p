#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_init(void);
void pwm_set_hardware(uint16_t ch1_ticks, uint16_t ch2_ticks);
void pwm_set_software(uint16_t ail2_ticks, uint16_t rudder_ticks);
void pwm_process_software(void);

#endif