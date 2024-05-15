#include <Arduino.h>
#include "main.hpp"
#include "active_object.h"
#include "lcd.h"
#include "rotary.h"

/*Main application object */

static uint16_t process_event_value(uint8_t rotary_value, uint8_t back_sig);
static void proobject_event_dispatcher(proobject_t *const mobj,event_t const *const e);

static void dispatch_temp_humi_state_sig(proobject_user_event_t *ue);

static proobject_t A0s;

static rotary_t rotary;


static uint8_t button = 0;;

void setup() {
  // put your setup code here, to run once:
  timer_isr_init();
  Serial.begin(9600);
  GPIO_init();
  lcd_init();
  rotary_init_esp32();
  proobject_init(&A0s);
}

void loop() {
//delay(100);
uint16_t proevent;

uint8_t rotarysig = 0;

static uint32_t current_time = millis();
static proobject_tick_event_t te;
  // put your main code here, to run repeatedly:
proobject_user_event_t ue;

reset_rotary(&rotary);
rotarysig = rotary_loop(&rotary);
reset_rotary(&rotary);

button = digitalRead(BUTTON_BACK_SIG);
proevent = process_event_value(rotarysig,button);
Serial.print(rotarysig);
/*Make Event*/
switch(proevent){
  case START_SIGNAL_DEFINE:
  {
    ue.super.sig = START_SIG;
  }
    break;
  case AUTO_MENU_BACK_SIGNAL_DEFINE:
  {
    ue.super.sig = TIMER_BACK_SIG;
  }
    break;
  case CAUTION_SET_DEFINE:
  {
    ue.super.sig = CAUTION_SIG;
  }
    break;
  case BACK_SIGNAL_DEFINE:
  {
    ue.super.sig = BACK_SIG;
  } 
    break;
  case SETTING_SIGNAL_DEFINE:
  {
    //ue.super.sig = SETTING_SIG;
    dispatch_temp_humi_state_sig(&ue);
  }
    break;
  case CW_SIGNAL_DEFINE:
  {
    ue.super.sig = CW_SIG;
  }
    break;
  case CCW_SIGNAL_DEFINE:
  {
    ue.super.sig = CCW_SIG;
  }  
    break;                                
}

proobject_event_dispatcher(&A0s,&ue.super);

//4. dispatch the time tick event for every 200ms
  if(millis() - current_time  >= TIME_TICK_CYCLE_DEFINE){
    //100ms has passed
    current_time = millis();
    te.super.sig = TIME_TICK_SIG;
    if(++te.ss > 10) te.ss = 1;
    proobject_event_dispatcher(&A0s,&te.super);
  }
  rotarysig = 0;
  button = 0;
}



static void proobject_event_dispatcher(proobject_t *const mobj,event_t const *const e){
  
  event_status_t status;
  proobject_state_t source, target;
  
  source = mobj->active_state;
  status = proobject_state_machine(mobj,e);

  if(status == EVENT_TRANSITION){
    target = mobj->active_state;
    event_t ee;
    //1. run the exit action for the source state
    ee.sig = EXIT;
    mobj->active_state = source;
    proobject_state_machine(mobj,&ee);

    //2. run the entry action for the target state
    ee.sig = ENTRY;
    mobj->active_state = target;
    proobject_state_machine(mobj,&ee);
  }
} 
static uint16_t process_event_value(uint8_t rotary_value, uint8_t back_sig){
    uint16_t signal;
    uint8_t internal_sig;
    internal_sig = get_internal_sig(); 
    signal = ((rotary_value<<4) | (back_sig<<3)) | internal_sig;
    reset_internal_sig();
    return signal;
}

static void dispatch_temp_humi_state_sig(proobject_user_event_t *ue){
   uint8_t temp_flag  = get_temp_flag();
   uint8_t humi_flag  = get_humi_flag();
   if(temp_flag == 1 && humi_flag ==0 ){
      ue->super.sig = TEMP_SET_SIG;
   }
   else if(temp_flag == 0 && humi_flag ==1){
      ue->super.sig = HUMI_SET_SIG;
   }
   else{
    ue->super.sig = SETTING_SIG;
   }
}