#include "main.hpp"
#include "user_define.h"
#include "active_object.h"
#include "lcd.h"
#include "LiquidCrystal_I2C.h"
#include "sht30.h"

/*define Signals*/
#define TIMER_BACK_SIG_DEFINE                   1

/*define value*/
/*Internal_signal*/
/*Signal for auto START*/
static uint16_t timer_auto_start_cnt = 0;
static bool isInIdle;
static uint8_t startsig;
/*Signal for auto back from Setting*/
static uint16_t timer_auto_back_cnt = 0;
static bool isInSetting;
static uint8_t autobacksig;

static uint16_t time_tick_cnt = 0;
static bool ActiveTimeTick;
static uint8_t time_tick_sig;

bool CheckSensor = true;

/*TEMP HUMI Signal enter */
static uint8_t temp_flag_state = 0;
static uint8_t humi_flag_state = 0;

//H/W timer defining (Pointer to the Structure)
static hw_timer_t * timer = NULL; 

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void led_build_notification(uint8_t pin,int ms);

static event_status_t proobject_state_handle_IDLE(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_SETTING(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_TEMP(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_HUMIDITY(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_RUN(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_CAUTION(proobject_t *const mobj, event_t const *const e);
static event_status_t proobject_state_handle_STOP(proobject_t *const mobj, event_t const *const e);

void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
 portENTER_CRITICAL_ISR(&timerMux);
/*Auto Start*/
if(isInIdle == true){
  if(timer_auto_start_cnt == 2800){ /*after 3s transistion to Run state*/
    startsig = 1;
    timer_auto_start_cnt = 0;
    
  }
  else{
    timer_auto_start_cnt++;
  }
}
else{
  startsig = 0;
  timer_auto_start_cnt = 0;
}
/*Auto Return main menu if in Setting State*/
if(isInSetting == true){
  if(timer_auto_back_cnt == 5000){  /*after 5s transistion to IDLE state*/
    autobacksig = 1;
    timer_auto_back_cnt = 0;
    isInSetting == false;
  }
  else{
    timer_auto_back_cnt++;
  }
}
else{
  autobacksig = 0;
  timer_auto_back_cnt = 0;
}
led_build_notification(LED_ISR_NOTIFICATION,LED_ISR_NOTIFICATION_DELAY_CALLBACK);
 portEXIT_CRITICAL_ISR(&timerMux);
}

void led_build_notification(uint8_t pin,int ms){
  static int i_led;
   if(i_led == ms){
    if(digitalRead(pin)==HIGH){
      digitalWrite(pin, LOW);
    }
    else{
      digitalWrite(pin, HIGH);
    }
    i_led = 0;
 }
 i_led++;
}

void timer_isr_init(){
  timer = timerBegin(0, ESP_PRESCALAR, true);           	  // timer 0, prescalar: 80, UP counting
  timerAttachInterrupt(timer, &onTimer, true); 	// Attach interrupt
  timerAlarmWrite(timer, ISR_CYCLE, true);  		      // Match value= 10000 for 10 ms. delay.
  timerAlarmEnable(timer);    
}

void GPIO_init(){
  pinMode(LED_BUILTIN,OUTPUT); 
}

// float convert_ms_to_s(){
//     return (float)interruptCounter/(float)MS_TO_S; 
// }

uint8_t get_internal_sig(){
    uint8_t isig;
    isig = (autobacksig<<1) | startsig;
    return isig;
}

void reset_internal_sig(){
  autobacksig = 0;
  startsig = 0;
  time_tick_sig = 0;
}

uint8_t get_temp_flag(){
   return temp_flag_state;
}

uint8_t get_humi_flag(){
   return humi_flag_state;
}

bool isinsettingstate(){
  return isInSetting;
}

void proobject_init(proobject_t *mobj){
    event_t ee;
    mobj->sensors.c_humi = DEFAULT_HUMI;
    mobj->sensors.c_temp = DEFAULT_TEMP;
    mobj->sensors.p_humi = DEFAULT_HUMI;
    mobj->sensors.p_temp = DEFAULT_TEMP;
    mobj->active_state = IDLE;
    /*Checking states*/
    isInIdle = false;
    isInSetting = false;
    ee.sig = ENTRY;
    proobject_state_machine(mobj,&ee);
}

event_status_t proobject_state_machine(proobject_t *const mobj, event_t const *const e){
  switch (mobj->active_state){
    case IDLE:
    {
      return proobject_state_handle_IDLE(mobj,e);
    }
      break;
    case SETTING:
    {
      return proobject_state_handle_SETTING(mobj,e);
    }
      break;
    case TEMP:
    {
      return proobject_state_handle_TEMP(mobj,e);
    }
      break;
    case HUMIDITY:
    {
      return proobject_state_handle_HUMIDITY(mobj,e);
    }
      break;
    case RUN:
    {
      return proobject_state_handle_RUN(mobj,e);
    }
      break;
    case STOP:
    {
      return proobject_state_handle_STOP(mobj,e);
    }
      break;
    case CAUTION:
    {
      return proobject_state_handle_CAUTION(mobj,e);
    }
      break;
  }
  return EVENT_IGNORED;
}

static event_status_t proobject_state_handle_IDLE(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:{
      Serial.print("IDLE_ENTRY\n");
      timer_auto_start_cnt = 0;
      isInIdle = true;
      //ActiveTimeTick = true;
      reset_lcd_start_couting();
      lcd_display(LCD_IDLE,mobj->slt,0);
      return EVENT_HANDLED;
    }
    case EXIT:{
      Serial.print("IDLE_EXIT\n");
      lcd_clear();
      isInIdle = false;
      return EVENT_HANDLED;
    }
    case TIME_TICK_SIG:
    {
      lcd_display(LCD_IDLE,mobj->slt,0);
      return EVENT_HANDLED;
    }
    case SETTING_SIG:{
      mobj->active_state = SETTING;
      return EVENT_TRANSITION;
    }
    case START_SIG:{
      mobj->active_state = RUN;
      return EVENT_TRANSITION;
    }
    case CAUTION_SIG:{
      mobj->active_state = CAUTION;
      return EVENT_TRANSITION;
    }
  }
  return EVENT_IGNORED;
}

static event_status_t proobject_state_handle_SETTING(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:{
      isInSetting = true;
      Serial.print("SETTING_ENTRY\n");
      temp_flag_state = 1;
      humi_flag_state = 0;
      lcd_display(LCD_SETTING,0,0);
      lcd_display(LCD_SETTING,1,0);
      return EVENT_HANDLED;
    }
    case EXIT:{
      Serial.print("SETTING_EXIT\n");
      temp_flag_state = 0;
      humi_flag_state = 0;
      autobacksig = 0;
      isInSetting = false;
      lcd_clear();
      return EVENT_HANDLED;
    }
    case CW_SIG:
    {
      isInSetting = true;
      timer_auto_back_cnt = 0;
      Serial.print("\n");
      Serial.print("CW_SIG\n");
      //lcd_clear();
      lcd_display(LCD_SETTING,2,0);
      temp_flag_state = 0;
      humi_flag_state = 1;
      return EVENT_HANDLED;
    }
    case CCW_SIG:
    {
      isInSetting = true;
      timer_auto_back_cnt = 0;
      Serial.print("\n");
      Serial.print("CCW_SIG\n");
      //lcd_clear();
      lcd_display(LCD_SETTING,1,0);
      temp_flag_state = 1;
      humi_flag_state = 0;
      return EVENT_HANDLED;
    }
    case TEMP_SET_SIG:
    {
      mobj->active_state = TEMP;
      return EVENT_TRANSITION;
    }
    case HUMI_SET_SIG:
    {
      mobj->active_state = HUMIDITY;
      return EVENT_TRANSITION;
    }
    case BACK_SIG:
    {
      mobj->active_state = IDLE;
      return EVENT_TRANSITION;
    }
    case TIMER_BACK_SIG:
    {
      mobj->active_state = IDLE;
      return EVENT_TRANSITION;
    }
  }
  return EVENT_IGNORED;
}

static event_status_t proobject_state_handle_TEMP(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:{
      Serial.print("TEMP_ENTRY\n");
      lcd_display(LCD_TEMP,0,0);
      lcd_display(LCD_TEMP,1,mobj->sensors.c_temp);
      return EVENT_HANDLED;
    }
    case EXIT:{
      Serial.print("TEMP_EXIT\n");
      return EVENT_HANDLED;
    }
    case CW_SIG:
    {
      Serial.print("CW_SIG\n");
      mobj->sensors.c_temp+=0.2;
      lcd_display(LCD_TEMP,1,mobj->sensors.c_temp);
      return EVENT_HANDLED;
    }
    case CCW_SIG:
    {
      Serial.print("CCW_SIG\n");
      mobj->sensors.c_temp-=0.2;
      lcd_display(LCD_TEMP,1,mobj->sensors.c_temp);
      return EVENT_HANDLED;
    }
    case SETTING_SIG:{
      Serial.print("SETTING_SIG\n");
      mobj->sensors.p_temp = mobj->sensors.c_temp;
      lcd_display(LCD_TEMP,2,mobj->sensors.p_temp);
      return EVENT_HANDLED;
    }
    case BACK_SIG:{
      Serial.print("BACK_SIG\n");
      mobj->active_state = SETTING; 
      return EVENT_TRANSITION;
    }
  }
  return EVENT_IGNORED; 
}

static event_status_t proobject_state_handle_HUMIDITY(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:
    {
      Serial.print("HUMIDITY_ENTRY\n");
      lcd_display(LCD_HUMIDITY,0,0);
      lcd_display(LCD_HUMIDITY,1,mobj->sensors.c_humi);
      return EVENT_HANDLED;
    }
    case EXIT:
    {
      Serial.print("HUMIDITY_EXIT\n");
      return EVENT_HANDLED;
    }
    case CW_SIG:
    {
      Serial.print("CW_SIG\n");
      mobj->sensors.c_humi+=0.2;
      lcd_display(LCD_HUMIDITY,1,mobj->sensors.c_humi);
      return EVENT_HANDLED;
    }
    case CCW_SIG:
    {
      Serial.print("CCW_SIG\n");
      mobj->sensors.c_humi-=0.2;
      lcd_display(LCD_HUMIDITY,1,mobj->sensors.c_humi);
      return EVENT_HANDLED;
    }
    case SETTING_SIG:{
      Serial.print("SETTING_SIG\n");
      mobj->sensors.p_humi = mobj->sensors.p_humi;
      lcd_display(LCD_HUMIDITY,2,mobj->sensors.p_humi);
      return EVENT_HANDLED;
    }
    case BACK_SIG:{
      Serial.print("BACK_SIG\n");
      mobj->active_state = SETTING; 
      return EVENT_TRANSITION;
    }
  }
  return EVENT_IGNORED; 
}

static event_status_t proobject_state_handle_RUN(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:
    {
      Serial.print("RUN_ENTRY\n");
      mobj->isFirstInRun = true;
      mobj->counting = 0;
      lcd_display(LCD_RUN,0,0);
      delay(100);
      return EVENT_HANDLED;
    }
    case EXIT:
    {
      Serial.print("RUN_EXIT\n");
      mobj->isFirstInRun = false;
      lcd_clear();
      return EVENT_HANDLED;
    }
    case TIME_TICK_SIG:
    {
      //delay(100);
      Serial.print("RUN_TIMETICK\n");
      lcd_display(LCD_RUN,1,0);
      return EVENT_IGNORED;
    }
    case SETTING_SIG:
    {
      mobj->active_state = SETTING;
      return EVENT_TRANSITION; 
    }
    // case BACK_SIG:
    // {
    //   mobj->active_state = STOP;
    //   return EVENT_TRANSITION; 
    // }
    case CAUTION_SIG:
    {
      return EVENT_TRANSITION;  
    } 
  }
  return EVENT_IGNORED; 
}

static event_status_t proobject_state_handle_STOP(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:
    {
      Serial.print("STOP_ENTRY\n");
      return EVENT_HANDLED;
    }
    case EXIT:
    {
      Serial.print("STOP_EXIT\n");
      return EVENT_HANDLED;
    }
    case BACK_SIG:
    {
      Serial.print("BACK_SIG\n"); 
      mobj->active_state = RUN;
      return EVENT_TRANSITION;
    }
    case SETTING_SIG:
    {
      Serial.print("SETTING_SIG\n"); 
      mobj->active_state = SETTING;
      return EVENT_TRANSITION;
    }  
  }
  return EVENT_IGNORED;
}

static event_status_t proobject_state_handle_CAUTION(proobject_t *const mobj, event_t const *const e){
  switch(e->sig){
    case ENTRY:
    {
      Serial.print("CAUTION_ENTRY\n"); 
      return EVENT_HANDLED;
    }
    case EXIT:
    {
      Serial.print("CAUTION_EXIT\n"); 
      return EVENT_HANDLED;
    }
    case BACK_SIG:
    {
      Serial.print("BACK_SIG\n"); 
      mobj->active_state = IDLE;
      return EVENT_TRANSITION;
    }
  }
  return EVENT_IGNORED; 
}