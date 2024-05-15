#ifndef     ROTARY_H
#define     ROTARY_H

#include "Arduino.h"

typedef struct{
    uint8_t rotary_previous;
    uint8_t rotary_current;
    uint8_t button_value;
    uint8_t ccw_value;
    uint8_t cw_value; 
    uint8_t sig;
}rotary_t;

void rotary_init(rotary_t *rotary);
void reset_rotary(rotary_t *rotary);
uint8_t rotary_read_button_status();
uint8_t rotary_read_phase_A_status();
uint8_t rotary_read_phase_B_status();

uint8_t process_rotary_bit_value(rotary_t *rotary);
uint8_t process_cw_ccw_button_rotaty(rotary_t *rotary);


void rotary_init_esp32();
uint8_t rotary_loop(rotary_t *rotary);
void rotary_onButtonClick(rotary_t *rotary);

#endif