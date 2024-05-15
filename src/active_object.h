#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include "main.hpp"

void timer_isr_init();

/*return s after timer_isr_init is called*/
float convert_ms_to_s();

uint8_t get_internal_sig();
void reset_internal_sig();
// uint8_t get_start_sig_value();
// uint8_t get_auto_back_sig_value();

void GPIO_init();


typedef enum{
    NOT_PRESSED,
    BOUNCE,
    PRESSED,
}button_state_t;


typedef enum {
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
}event_status_t;

/* Signals of the application*/
typedef enum{
    BACK_SIG = 1,
    SETTING_SIG,
    CW_SIG,
    CCW_SIG,
    TEMP_SET_SIG,
    HUMI_SET_SIG,
    START_SIG,
    CAUTION_SIG,
/*Internal transition sig*/
    TIME_TICK_SIG,
/*Timming Sigs*/
    TIMER_BACK_SIG,
/* Internal activity signals */
    ENTRY,
    EXIT
}proobject_signal_t;


typedef enum{
    IDLE,
    SETTING,
    TEMP,
    HUMIDITY,
    RUN,
    STOP,
    CAUTION,
}proobject_state_t;

//forward declarations
struct proobject_tag;
struct event_tag;

//typedef  event_status_t (*proobject_state_t)(struct proobject_tag  *const , struct event_tag const *const);

/*struct for sensor*/

enum{
    TEMP_UNIT_C = 1,
    TEMP_UNIT_F = 2,
};

typedef struct{
    /*temp*/
    float c_temp;
    float p_temp;
    uint8_t temp_unit;
    /*humidity*/
    float c_humi;
    float p_humi;
}sensors_t;


/* Main application structure */
typedef struct proobject_tag {
    sensors_t sensors;
    uint8_t slt;
    uint32_t curr_time;
    uint32_t elapsed_time;
    uint32_t pro_time;
    bool isFirstInRun;
    uint32_t counting;
    int32_t cwcnt;
    int32_t ccwcnt;
    proobject_state_t active_state;
}proobject_t;

/*Generic(Super) event structure */
typedef struct event_tag{
    uint8_t sig;
}event_t;

/* For user generated events */
typedef struct{
    event_t super;
}proobject_user_event_t;

/* For tick event */
typedef struct{
    event_t super;
    uint8_t ss;
}proobject_tick_event_t;

/* For menu event */
typedef struct{
    event_t super;
    uint8_t ss;
}proobject_setting_menu_event_t;


void proobject_init(proobject_t *mobj);
bool isinsettingstate();
uint8_t get_temp_flag();
uint8_t get_humi_flag();
event_status_t proobject_state_machine(proobject_t *const mobj, event_t const * const e);


#endif //ACTIVE_OBJECT_H