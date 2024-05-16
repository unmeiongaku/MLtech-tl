#ifndef LCD_H
#define LCD_H

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
// #include "user_define.h"

typedef enum{
    LCD_IDLE,
    LCD_SETTING,
    LCD_TEMP,
    LCD_HUMIDITY,
    LCD_RUN,
    LCD_CAUTION,
    LCD_STOP
}lcd_t;


void lcd_init();
void lcd_clear();
void lcd_set_cursor(uint8_t col, uint8_t row);
void reset_lcd_start_couting();

uint8_t lcd_display(lcd_t lcd,uint8_t slt,float setvalue);


#endif