#include "../include/Video.h"

// Minimal vidmodes table to satisfy linker for headless builds.
// Each entry has 17 properties as declared in Video.h's vmodeproperties enum.
// Values chosen as conservative defaults (320x240, vDiv=1, small porch/sync values).
const unsigned short int vidmodes[][17] = {
  {320, 240, 1, 16, 96, 48, 10, 2, 33, 0, 0, 0, 0, 0, 0, 1, 1}
};
