#ifndef __LED_INCLUDED__
#define __LED_INCLUDED__

#include "data.h"
#include "tlc59731.h"
#include "Ticker.h"

void LedInit(int pin);
void LedBlink(uint32_t color, uint32_t period); // Infinite blink
void LedBlink(uint32_t color, uint32_t period, uint8_t nb_blink);
void LedRainbow(uint32_t delay_ms);
void LedFadeIn(uint32_t color, uint32_t delay_ms);
void LedFadeOut(uint32_t color, uint32_t delay_ms);
void LedFade(uint32_t color, uint32_t delay_ms); // Infinite fade
void LedFade(uint32_t color, uint32_t delay_ms, uint8_t nb_fade);
void LedOn(uint32_t color);
void LedOff();

#endif

