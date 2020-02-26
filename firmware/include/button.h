#ifndef LUEUR_FW_BUTTON_H_
#define LUEUR_FW_BUTTON_H_

#include "data.h"
#include "led.h"

enum ButtonStates {
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED,
    BUTTON_PRESSED_RELEASED,
    BUTTON_DOUBLE_PRESSED,
    BUTTON_LONG_PRESSED,
};

void ButtonInit(int pin);
void ButtonProcess();
void ButtonResetCallBack(); // Weak function
void ButtonClickCallBack(); // Weak function
void ButtonDoubleClickCallBack(); // Weak function
void ButtonLongPressCallBack(); // Weak function

#endif
