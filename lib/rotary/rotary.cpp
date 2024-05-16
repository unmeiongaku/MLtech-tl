#include "rotary.h"
#include "Arduino.h"
#include "user_define.h"
#include "AiEsp32RotaryEncoder.h"
//#include "active_object.h"


#define ROTARY_ENCODER_VCC_PIN -1

//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 1

static AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(DEFINE_ROTARY_PHASE_A_INPUT, DEFINE_ROTARY_PHASE_B_INPUT, DEFINE_ROTARY_BUTTON_INPUT, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

static long precwccw = 0;
static long curcwccw = 0;

void rotary_onButtonClick(rotary_t *rotary)
{
	static unsigned long lastTimePressed = 0;
	//ignore multiple press in that time milliseconds
	if (millis() - lastTimePressed < 500)
	{
		return;
	}
	lastTimePressed = millis();
	Serial.print("button pressed ");
    rotary->button_value = 1;
	Serial.print(millis());
	Serial.println(" milliseconds after restart");
}

uint8_t rotary_loop(rotary_t *rotary)
{
	//dont print anything unless value changed
    //precwccw = rotaryEncoder.readEncoder();
	if (rotaryEncoder.encoderChanged())
	{
        curcwccw = rotaryEncoder.readEncoder();
        if(curcwccw > precwccw){
            Serial.print("CW\n");
            rotary->cw_value = 1;
            rotary->ccw_value = 0;
        }
        else if(curcwccw < precwccw){
            Serial.print("CCW\n");
            rotary->cw_value = 0;
            rotary->ccw_value = 1;
        }
		Serial.print("Value: ");
		Serial.println(rotaryEncoder.readEncoder());
	}
    else{
        rotary->cw_value = 0;
        rotary->ccw_value = 0;
    }
    precwccw = rotaryEncoder.readEncoder();
	if (rotaryEncoder.isEncoderButtonClicked())
	{
		rotary_onButtonClick(rotary);
	}
    rotary->sig = (rotary->ccw_value<<2)|(rotary->cw_value<<1) | (rotary->button_value);
    reset_rotary(rotary);
    return rotary->sig;
}

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}


void rotary_init_esp32(){
	rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
    bool circleValues = false;
    rotaryEncoder.setBoundaries(-1000, 1000, circleValues);
    rotaryEncoder.disableAcceleration();    
}


void reset_rotary(rotary_t *rotary){
    rotary->button_value = 0;
    rotary->ccw_value =0 ;
    rotary->cw_value = 0;
}

uint8_t rotary_read_button_status(){
    uint8_t button;
    button = digitalRead(DEFINE_ROTARY_BUTTON_INPUT);
    if(button == 0){
        button = 1;
    }
    else if(button == 1){
        button = 0;
    }
    return button;
}

uint8_t rotary_read_phase_A_status(){
    return digitalRead(DEFINE_ROTARY_PHASE_A_INPUT); 
}

uint8_t rotary_read_phase_B_status(){
    return digitalRead(DEFINE_ROTARY_PHASE_B_INPUT); 
}

// void rotary_init(rotary_t *rotary){
//     pinMode(DEFINE_ROTARY_PHASE_A_INPUT,INPUT);
//     pinMode(DEFINE_ROTARY_PHASE_B_INPUT,INPUT);
//     pinMode(DEFINE_ROTARY_BUTTON_INPUT,INPUT_PULLUP);
//     rotary->rotary_previous = rotary_read_phase_A_status();
// }

// uint8_t process_rotary_bit_value(rotary_t *rotary){
//         rotary->sig = 0;
//         rotary->sig = (rotary->ccw_value<<2)|(rotary->cw_value<<1) | (rotary->button_value);
//     return rotary->sig;
// }

// uint8_t process_cw_ccw_button_rotaty(rotary_t *rotary){
//     rotary->button_value = rotary_read_button_status();
//     rotary->rotary_current = rotary_read_phase_A_status();
//     if(rotary->rotary_current != rotary->rotary_previous){
//         uint8_t B = rotary_read_phase_B_status();
//         if (B != rotary->rotary_current){
//             rotary->cw_value = 1;
//             rotary->ccw_value = 0;
//             Serial.print("CW\n");
//         }
//         else{
//             rotary->cw_value = 0;
//             rotary->ccw_value = 1;
//             Serial.print("CCW\n");
//         }
//     }
//     else{
//             rotary->cw_value = 0;
//             rotary->ccw_value = 0;
//             //Serial.print("Not Rotate");
//     }
//     uint8_t rsig = process_rotary_bit_value(rotary);
//     return rsig;
// }

