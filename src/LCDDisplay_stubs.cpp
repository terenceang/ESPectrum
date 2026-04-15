#include "../include/Video.h"  // must come first — defines USE_LCD
#include "LCDDisplay.h"

// No-op stubs for headless (non-LCD) builds.
#ifndef USE_LCD
void LCDDisplay::Init()  {}
void LCDDisplay::Flush() {}
#endif
