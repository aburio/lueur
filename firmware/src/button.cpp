#include "button.h"

struct Button {
  ButtonStates state;
  uint32_t press_time;
  int pin;
};

Button button;

void ButtonInit(int pin) {
  uint32_t start = millis();
  int button_value;

  button.state = BUTTON_RELEASED;
  button.press_time = 0;
  button.pin = pin;
  
  pinMode(button.pin, INPUT);
  delay(100);

  button_value = digitalRead(button.pin);

  if (button_value == LOW) {
    while ((millis() < start + BUTTON_RESET_PERIOD) && (button_value == LOW)) {
      button_value = digitalRead(button.pin);
      yield();
    }
    if (button_value == LOW) {
      ButtonResetCallBack();
    }
  }
}

void ButtonProcess() {
  int button_value = digitalRead(button.pin);
  uint32_t now = millis();

  // Button is in default state
  if (button.state == BUTTON_RELEASED) {
    if (button_value == LOW) {
      button.state = BUTTON_PRESSED;
      button.press_time = now;
    }
  }
  // Button pressed ?
  else if (button.state == BUTTON_PRESSED) {
    if ((button_value == HIGH) && (now - button.press_time < BUTTON_DEBOUNCE_PERIOD)) {
      button.state = BUTTON_RELEASED;
      // bounce press we set the default state back
    }
    else if (button_value == HIGH) {
      button.state = BUTTON_PRESSED_RELEASED;
      // button pressed and released : one click
    }
    else if ((button_value == LOW) && (now - button.press_time > BUTTON_LONG_PRESS_PERIOD)) {
      ButtonLongPressCallBack();
      button.state = BUTTON_LONG_PRESSED;
      // button long press start
    }
  }
  // Button second click or simple click ?
  else if (button.state == BUTTON_PRESSED_RELEASED) {
    if ((now - button.press_time) > BUTTON_DOUBLE_CLICK_PERIOD) {
      ButtonClickCallBack();
      button.state = BUTTON_RELEASED;
    }
    else if (button_value == LOW) {
      button.state = BUTTON_DOUBLE_PRESSED;
    }
  }
  // Button second click released ?
  else if (button.state == BUTTON_DOUBLE_PRESSED) {
    if (button_value == HIGH) {
      ButtonDoubleClickCallBack();
      button.state = BUTTON_RELEASED;
    }
  }
  else if (button.state == BUTTON_LONG_PRESSED) {
    if (button_value == HIGH) {
      button.state = BUTTON_RELEASED;
    }
  }
}

__attribute__ ((weak)) void ButtonResetCallBack() {
  // To be defined by user
}

__attribute__ ((weak)) void ButtonClickCallBack() {
  // To be defined by user
}

__attribute__ ((weak)) void ButtonDoubleClickCallBack() {
  // To be defined by user
}

__attribute__ ((weak)) void ButtonLongPressCallBack() {
  // To be defined by user
}

