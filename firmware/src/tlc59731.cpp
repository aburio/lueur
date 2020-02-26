#include "tlc59731.h"

Tlc59731::Tlc59731() {
  pin_number_ = -1;
  red_value_ = 0;
  green_value_ = 0;
  blue_value_ = 0;
  brightness_value_ = 255;
  pinMode(pin_number_, OUTPUT);
}

Tlc59731::Tlc59731(int pin_number) {
  pin_number_ = pin_number;
  red_value_ = 0;
  green_value_ = 0;
  blue_value_ = 0;
  brightness_value_ = 255;
  pinMode(pin_number_, OUTPUT);
}

void Tlc59731::setColor(uint32_t color) {
  red_value_ = (uint8_t) (color >> 16);
  green_value_ = (uint8_t) (color >>  8);
  blue_value_ = (uint8_t) color;
}

void Tlc59731::setColor(uint8_t r, uint8_t g, uint8_t b) {
  red_value_ = r;
  green_value_ = g;
  blue_value_ = b;
}

void Tlc59731::setBrightness(uint8_t brightness) {
  if (brightness_value_ != brightness) {
    brightness_value_ = brightness;
  }
}

uint32_t Tlc59731::getColor() const {
  return ((red_value_ << 16) | (green_value_ << 8) | blue_value_);
}

uint8_t Tlc59731::getRedValue() const {
  return red_value_;
}

uint8_t Tlc59731::getGreenValue() const {
  return green_value_;
}

uint8_t Tlc59731::getBlueValue() const {
  return blue_value_;
}

void Tlc59731::clear() {
  setColor(0);
}

void Tlc59731::show() {
  uint8_t red = (red_value_ * brightness_value_) >> 8;
  uint8_t green = (green_value_ * brightness_value_) >> 8;
  uint8_t blue = (blue_value_ * brightness_value_) >> 8;

  sendByte(WRITE_COMMAND);
  sendByte(red);
  sendByte(green);
  sendByte(blue);
}

void ICACHE_RAM_ATTR Tlc59731::sendByte(uint8_t byte_value) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(pin_number_, HIGH);
    delayMicroseconds(DELAY);
    digitalWrite(pin_number_, LOW);
    delayMicroseconds(DELAY);

    if (byte_value & (1 << i))
    { // write "1" data bit
      digitalWrite(pin_number_, HIGH);
      delayMicroseconds(DELAY);
      digitalWrite(pin_number_, LOW);
      delayMicroseconds(T_CYCLE_1);
    }
    else
    { // for "0" data bit simply let timeout the cylce
      delayMicroseconds(T_CYCLE_0);
    }
  }
}

