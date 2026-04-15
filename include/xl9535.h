#pragma once
#include <stdint.h>

// Minimal XL9535 helper for backlight control (Port0 Bit0)
namespace XL9535 {
  void init();
  void setBacklight(bool on);
}
