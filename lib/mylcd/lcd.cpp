#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "lcd.h"
#include "user_define.h"
#include "stdlib.h"

//static uint8_t lcd_idle_handle(uint8_t slt);

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COL, LCD_ROW);

static float lcd_run_cnt = 3.0;

static float temp,humi;

static uint8_t lcd_idle_handle(uint8_t slt);
static uint8_t lcd_setting_handle(uint8_t slt);
static uint8_t lcd_temp_handle(uint8_t slt, float value);
static uint8_t lcd_run_handle(uint8_t slt);
static uint8_t lcd_humidity_handle(uint8_t slt, float value);
static uint8_t lcd_caution_handle(uint8_t slt);
static uint8_t lcd_stop_handle(uint8_t slt);

void lcd_init(){
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("Hello, world!");
    delay(1000);
    lcd.clear();
}

void lcd_clear(){
    lcd.clear();
}

void lcd_set_cursor(uint8_t col, uint8_t row){
    lcd.setCursor(col,row);
}

void reset_lcd_start_couting(){
    lcd_run_cnt = 3.2;
}

uint8_t lcd_display(lcd_t lcd,uint8_t slt,float setvalue){
    switch(lcd){
        case LCD_IDLE:
        {
            lcd_idle_handle(slt);
        } 
            break;
        case LCD_SETTING:
        {
            lcd_setting_handle(slt);
        } 
            break;
        case LCD_TEMP:
        {
            lcd_temp_handle(slt,setvalue);
        } 
            break;
        case LCD_HUMIDITY:
        {
            lcd_humidity_handle(slt,setvalue);
        } 
            break;
        case LCD_RUN:
        {
            lcd_run_handle(slt);
        } 
            break;
        case LCD_CAUTION:
        {
            lcd_caution_handle(slt);
        } 
            break;
        case LCD_STOP:
        {
            lcd_stop_handle(slt);
        } 
            break;
    }
    return 0;
}

static uint8_t lcd_idle_handle(uint8_t slt){
    switch(slt){
        case 0:
        {   
            lcd.setCursor(4,0);
            lcd.print("BO DIEU KHIEN");
            lcd.setCursor(6,1);
            lcd.print("TU LANH");
            lcd.setCursor(0,3);
            lcd.print(lcd_run_cnt);
            lcd.setCursor(13,3);
            lcd.print("CAI DAT");
            lcd_run_cnt = lcd_run_cnt-((float)((float)TIME_TICK_CYCLE_DEFINE/1000.0)*2.0);
        }
            break;
        default:
        break;
    }
    return 0;
}

static uint8_t lcd_setting_handle(uint8_t slt){
    switch(slt){
        case 0:
        {
            lcd.setCursor(0,0);
            lcd.print("CAI DAT NHIET DO");
            lcd.setCursor(0,1);
            lcd.print("CAI DAT DO AM");
            lcd.setCursor(0,3);
            lcd.print("BACK");
            lcd.setCursor(14,3);
            lcd.print("SELECT");
        }
            break;
        case 1:
        {
            lcd.setCursor(19,1);
            lcd.print(" ");
            lcd.setCursor(19,0);
            lcd.print("*");
        }
            break;
        case 2:
        {
            lcd.setCursor(19,0);
            lcd.print(" ");
            lcd.setCursor(19,1);
            lcd.print("*");
        }
    }
    return 0;
}

static uint8_t lcd_temp_handle(uint8_t slt, float value){
    switch(slt){
    case 0:
    {
        lcd.setCursor(3,0);
        lcd.print("TEMPERATURE SET");
        lcd.setCursor(12,2);
        lcd.print("C");
        lcd.setCursor(0,3);
        lcd.print("BACK");
        lcd.setCursor(15,3);
        lcd.print(" SAVE");
    }
        break;
    case 1:
    {   
        int floattoint = (int)value;
        if(floattoint<10){
            lcd.setCursor(7,2);
            lcd.print(" ");
            lcd.setCursor(8,2);
            lcd.print(value);
        }
        else{
            lcd.setCursor(7,2);
            lcd.print(value);
        }
        lcd.setCursor(15,3);
        lcd.print(" SAVE");
    }
        break;
    case 2:
    {
        lcd.setCursor(15,3);
        lcd.print("SAVED");
    }
        break;
    }
    return 0;
}

static uint8_t lcd_humidity_handle(uint8_t slt, float value)
{
    switch(slt){
    case 0:
    {
        lcd.setCursor(4,0);
        lcd.print("HUMIDITY SET");
        lcd.setCursor(12,2);
        lcd.print("RH");
        lcd.setCursor(0,3);
        lcd.print("BACK");
        lcd.setCursor(15,3);
        lcd.print(" SAVE");
    }
        break;
    case 1:
    {   
        int floattoint = (int)value;
        if(floattoint<10){
            lcd.setCursor(7,2);
            lcd.print(" ");
            lcd.setCursor(8,2);
            lcd.print(value);
        }
        else{
            lcd.setCursor(7,2);
            lcd.print(value);
        }
        lcd.setCursor(15,3);
        lcd.print(" SAVE");
    }
        break;
    case 2:
    {
        lcd.setCursor(15,3);
        lcd.print("SAVED");
    }
        break;
    }
    return 0;
}

static uint8_t lcd_run_handle(uint8_t slt){
    switch(slt){
        case 0:
        {
            lcd.setCursor(6,0);
            lcd.print("RUNNING!");
            lcd.setCursor(0,1);
            lcd.print("TEMP: ");
            lcd.setCursor(12,1);
            lcd.print("C");
            lcd.setCursor(0,2);
            lcd.print("HUMI: ");
            lcd.setCursor(12,2);
            lcd.print("%RH");
            lcd.setCursor(0,3);
            lcd.print("STOP");
            lcd.setCursor(13,3);
            lcd.print("SETTING");
        }
            break;
        case 1:
        {
            // lcd.setCursor(4,0);
            // lcd.print("BO DIEU KHIEN");
            // lcd.setCursor(3,1);
            // lcd.print("DANG HOAT DONG");
            // lcd.setCursor(0,3);
            // lcd.print("ND:");
            lcd.setCursor(6,1);
            lcd.print(temp);
            lcd.setCursor(6,2);
            lcd.print(humi);
            // lcd.setCursor(9,3);
            // lcd.print("DA:");
            // lcd.setCursor(18,3);
            // lcd.print("ST");
            temp = temp + 0.1;
            humi = humi + 0.1;
        }
            break;
    }

    return 0; 
}

static uint8_t lcd_caution_handle(uint8_t slt){
    switch(slt){
    case 0:
    {
        lcd.setCursor(0,0);
        lcd.print("Can Not Read SHT30");
        lcd.setCursor(0,1);
        lcd.print("Sensor. Please Check");
        lcd.setCursor(0,2);
        lcd.print("Your Connection. ");
        lcd.setCursor(0,3);
        lcd.print("BACK");
    }
        break;
    case 1:
    {

    }
        break;
    case 2:
    {

    }
        break;
    }
    return 0; 
}

static uint8_t lcd_stop_handle(uint8_t slt){
    switch(slt){
    case 0:
    {
        lcd.setCursor(3,1);
        lcd.print("STOP WORKING!");
        lcd.setCursor(0,3);
        lcd.print("RUN");
        lcd.setCursor(13,3);
        lcd.print("SETTING");
    }
        break;
    }
    return 0; 
}

