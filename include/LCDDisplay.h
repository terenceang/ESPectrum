#pragma once

#ifdef USE_LCD
#include <stdint.h>

class LCDDisplay {
public:
  static void Init();
  static void Flush();
};
#endif
