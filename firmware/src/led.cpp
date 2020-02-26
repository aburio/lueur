#include "led.h"

struct LedParameters {
  uint32_t nb_iteration = 0;
  uint32_t iteration = 0;
};

LedParameters led_param;
Ticker led_tick;

Tlc59731 led;

void LedInit(int pin) {
  led = Tlc59731(pin);
  led.setBrightness(LED_BRIGHT);
  led.clear();
  led.show();
}

void ICACHE_RAM_ATTR LedToggle(uint32_t color) {
  if ((led_param.nb_iteration != 0) && (led_param.iteration >= led_param.nb_iteration)) {
    LedOff();
  }
  else if (led.getColor() == 0) {
    led.setColor(color);
  }
  else {
    led.clear();
  }

  led.show();

  if (led_param.nb_iteration != 0) {
    led_param.iteration++;
  }
}

void LedBlink(uint32_t color, uint32_t period) {
  LedBlink(color, period, 0);
}

void LedBlink(uint32_t color, uint32_t period, uint8_t nb_blink) {
  led_tick.detach(); //Stop previous blink or rainbow
  if (nb_blink != 0) {
    led_param.nb_iteration = nb_blink * 2;
  }
  else {
    led_param.nb_iteration = 0;
  }
  led_param.iteration = 0;
  led_tick.attach_ms(period, LedToggle, color);
}

void ICACHE_RAM_ATTR LedColorWheel() {
  if (led_param.iteration < 256) {
    byte WheelPos = (255 - led_param.iteration) & 255;
    if (WheelPos < 85) {
      led.setColor(255 - WheelPos * 3, 0, WheelPos * 3);
    } 
    else if (WheelPos < 170) {
      WheelPos -= 85;
      led.setColor(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else {
      WheelPos -= 170;
      led.setColor(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    led_param.iteration ++;
  }
  else {
    led_param.iteration = 0;
  }
  led.show();
}

void ICACHE_RAM_ATTR LedColorIn(uint32_t color) {
  uint8_t red = ((uint8_t)(color >> 16) * led_param.iteration) >> 8;
  uint8_t green = ((uint8_t) (color >>  8) * led_param.iteration) >> 8;
  uint8_t blue = ((uint8_t) color * led_param.iteration) >> 8;
  
  if (led_param.iteration < 255) {
    led_param.iteration += 3;
  }
  else {
    led_param.iteration = 0;
    led_tick.detach();
  }
  
  led.setColor(red, green, blue);
  led.show();
}

void ICACHE_RAM_ATTR LedColorOut(uint32_t color) {
  uint8_t red =  ((uint8_t)(color >> 16) * (255 - led_param.iteration)) >> 8;
  uint8_t green = ((uint8_t) (color >>  8) * (255 -led_param.iteration)) >> 8;
  uint8_t blue = ((uint8_t) color * (255 - led_param.iteration)) >> 8;
  
  if (led_param.iteration < 255) {
    led_param.iteration += 3;
  }
  else {
    led_param.iteration = 0;
    led_tick.detach();
  }
  
  led.setColor(red, green, blue);
  led.show();
}

void ICACHE_RAM_ATTR LedColorInOut(uint32_t color) {
  uint8_t red = (uint8_t) (color >> 16);
  uint8_t green = (uint8_t) (color >>  8);
  uint8_t blue = (uint8_t) color;

  if ((led_param.nb_iteration != 0) && (led_param.iteration > led_param.nb_iteration)) {
    LedOff();
  }
  else if (led_param.iteration % 2 == 0) {
    // Fade Out
    red = (red * (led.getRedValue() - 3)) >> 8;
    green = (green * (led.getGreenValue() - 3)) >> 8;
    blue = (blue * (led.getBlueValue() - 3)) >> 8;
    led.setColor(red, green, blue);
    if (led.getColor() == 0) {
      led_param.iteration++;
    }
  }
  else {
    // Fade In
    red = (red * (led.getRedValue() + 3)) >> 8;
    green = (green * (led.getGreenValue() + 3)) >> 8;
    blue = (blue * (led.getBlueValue() + 3)) >> 8;
    led.setColor(red, green, blue);
    if (led.getColor() == color) {
      led_param.iteration++;
    }
  }

  led.show();
}



void LedRainbow(uint32_t delay_ms) {
  led_tick.detach(); //Stop previous blink or rainbow
  led_param.iteration = 0;
  led_tick.attach_ms(delay_ms, LedColorWheel);
}

void LedFadeIn(uint32_t color, uint32_t delay_ms) {
  led_tick.detach();
  led_param.iteration = 0;
  led_tick.attach_ms(delay_ms, LedColorIn, color);
}

void LedFadeOut(uint32_t color, uint32_t delay_ms) {
  led_tick.detach();
  led_param.iteration = 0;
  led_tick.attach_ms(delay_ms, LedColorOut, color);
}

void LedFade(uint32_t color, uint32_t delay_ms) {
  LedFade(color, delay_ms, 0);
}

void LedFade(uint32_t color, uint32_t delay_ms, uint8_t nb_fade) {
  led_tick.detach();
  if (nb_fade != 0) {
    led_param.nb_iteration = nb_fade * 2;
  }
  else {
    led_param.nb_iteration = 0;
  }
  led_param.iteration = 1; // For consistency with fade count
  led_tick.attach_ms(delay_ms, LedColorInOut, color);
}

void LedOn(uint32_t color) {
  led_tick.detach(); //Stop previous blink or rainbow
  led.setColor(color);
  led.show();
}

void LedOff() {
  led_tick.detach();
  led.clear();
  led.show();
}
