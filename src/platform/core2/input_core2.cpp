#include <M5Unified.h>
#include "platform/input.h"

static m5::Button_Class& button_for(Verb verb){
    switch (verb) {
        case Verb::select:  return M5.BtnA;
        case Verb::confirm: return M5.BtnB;
        case Verb::cancel:  return M5.BtnC;
    }
    return M5.BtnA;
}

bool input_check_pressed(Verb verb){
    return button_for(verb).wasPressed();   // because wasPressed is a boolean we can return the value directly
}

bool input_check(Verb verb){
    return button_for(verb).isPressed();

}
bool input_check_released(Verb verb){
    return button_for(verb).wasReleased();
}