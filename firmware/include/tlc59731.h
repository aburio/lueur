#ifndef __TLC59731_INCLUDED__
#define __TLC59731_INCLUDED__

#include "data.h"

#define WRITE_COMMAND 0x3Au
// Timing
#define DELAY     1 // width a pulse on the wire (read: 1 unit!), period is 2 units
#define T_CYCLE_0 4 // time to wait for the cycle to complete (before the next bit can be sent) if a 0 was send (4 units)
#define T_CYCLE_1 1 // time to wait for the cycle to complete (before the next bit can be sent) if a 1 was send (1 unit)


class Tlc59731 {
  public :
    Tlc59731();
    Tlc59731(int pin_number);
    void setColor(uint32_t color);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(uint8_t brightness);
    uint32_t getColor() const;
    uint8_t getRedValue() const;
    uint8_t getGreenValue() const;
    uint8_t getBlueValue() const;
    void clear();
    void show();

  private :
    void ICACHE_RAM_ATTR sendByte(uint8_t byte_value);
  
    int pin_number_;
    uint8_t brightness_value_;
    uint8_t red_value_;
    uint8_t green_value_;
    uint8_t blue_value_;
};

#endif
