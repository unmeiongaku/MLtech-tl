#ifndef USER_DEFINE_H
#define USER_DEFINE_H

#include "Arduino.h"

/*Define Your LCD */
#define LCD_I2C_SELECT     1
/*    0x27                 0
*     0x3F                 1 
*/

#if LCD_I2C_SELECT == 0
#define LCD_I2C_ADDR      0x27
#elif LCD_I2C_SELECT == 1
#define LCD_I2C_ADDR      0x3F
#endif

#define LCD_TYPE       0
/*    2004                 0
*     1602                 1 
*/

#if LCD_TYPE == 0
#define    LCD_COL     20
#define    LCD_ROW     4
#elif LCD_TYPE == 1
#define    LCD_COL     16
#define    LCD_ROW     2
#endif

/*Define your SCT30 */
#define SHT30_I2C_ADDR      0X44


/*define Your isr*/
//ESP worked with 80mhz
//prescaler 80 
//CYCLE = 80MHZ/80 = 1MHZ

#define ESP_PRESCALAR       80

/*Select your ISR CYCLE*/
/*  1ms                     0
*   10ms                    1        
*   100ms                   2
*   1s                      3    
*/

#define TIME_TICK_CYCLE_DEFINE  100

#define SELECT_YOUR_ISR     0

#if  SELECT_YOUR_ISR == 0
#define ISR_CYCLE           1000
#define MS_TO_S             1000   
#elif   SELECT_YOUR_ISR == 1
#define ISR_CYCLE           10000
#define MS_TO_S             100 
#elif   SELECT_YOUR_ISR == 2
#define ISR_CYCLE           100000
#define MS_TO_S             10 
#elif   SELECT_YOUR_ISR == 3
#define ISR_CYCLE           1000000
#define MS_TO_S             1
#endif

#define LED_ISR_NOTIFICATION                    LED_BUILTIN
#define LED_ISR_NOTIFICATION_DELAY_CALLBACK     50


/*Define Caution Enable*/
#define CATION_SENSORS_ENABLE               0

/*Define Rotary*/
#define DEFINE_ROTARY_PHASE_A_INPUT        19            
#define DEFINE_ROTARY_PHASE_B_INPUT        18
#define DEFINE_ROTARY_BUTTON_INPUT         15

#define ROTARY_CW_SIG_DEFINE               1
#define ROTARY_CCW_SIG_DEFINE              2
#define ROTARY_SETTING_SIG_DEFINE          3

/*define Button*/
#define BUTTON_BACK_SIG                    13                                               

#define START_SIGNAL_DEFINE                    1    
#define AUTO_MENU_BACK_SIGNAL_DEFINE           2   //(1<<1)
#define CAUTION_SET_DEFINE                     4   //(1<<2) 
#define BACK_SIGNAL_DEFINE                     8   //(1<<3)
#define SETTING_SIGNAL_DEFINE                  16  //(1<<4) 
#define CW_SIGNAL_DEFINE                       32  //(1<<5) 
#define CCW_SIGNAL_DEFINE                      64  //(1<<6)

#define DEFAULT_TEMP                           24
#define DEFAULT_HUMI                           50


#define CONTROL_OUTPUT_GPIO_TEMP                8
#define CONTROL_OUTPUT_GPIO_HUMI                9

#endif