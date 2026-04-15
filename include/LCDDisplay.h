#pragma once

#ifdef USE_LCD
#include <stdint.h>

class LCDDisplay {
public:
  static void Init();
  static void Flush();
private:
  static uint16_t convertPixel(uint8_t pixel);
};
#endif
