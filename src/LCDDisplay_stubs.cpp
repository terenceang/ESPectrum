#include "../include/Video.h"

// Provide minimal no-op implementations for LCDDisplay used in headless builds.
// Include Video.h so `USE_LCD` and class declarations are active.
#ifdef USE_LCD
void LCDDisplay::Init() {}
void LCDDisplay::Flush() {}

uint16_t LCDDisplay::convertPixel(uint8_t pixel) { return (uint16_t)pixel; }
#endif
