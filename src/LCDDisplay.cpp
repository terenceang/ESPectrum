#include "LCDDisplay.h"
#ifdef USE_LCD

#include <TFT_eSPI.h>
#include "Video.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xl9535.h"

static uint16_t *stripBuffer = nullptr;
static const int LCD_WIDTH  = 320;
static const int LCD_HEIGHT = 240;
static TFT_eSPI tft = TFT_eSPI(LCD_WIDTH, LCD_HEIGHT);
static const int OFFSET_X   = 8;
static const int OFFSET_Y   = 16;
static const int ACTIVE_W   = 224;
static const int ACTIVE_H   = LCD_HEIGHT - OFFSET_Y; // lines that fit: 224
static const int STRIP_HEIGHT = 48;
static bool lcdInitialized = false;

void LCDDisplay::Init() {
  if (lcdInitialized)
    return;

  tft.begin();
  // Ensure the display is in landscape mode (320x240)
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Initialize XL9535 and enable backlight
  XL9535::init();
  XL9535::setBacklight(true);

  size_t allocCount = (size_t)ACTIVE_W * STRIP_HEIGHT;
  stripBuffer = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * allocCount, MALLOC_CAP_INTERNAL);
  if (!stripBuffer) {
    stripBuffer = (uint16_t *)malloc(sizeof(uint16_t) * allocCount);
  }

  lcdInitialized = true;
}

static uint16_t convertPixel(uint8_t pixel) {
  uint8_t color = pixel & 0x3F;
  uint8_t r2 = color & 0x03;
  uint8_t g2 = (color >> 2) & 0x03;
  uint8_t b2 = (color >> 4) & 0x03;
  return (uint16_t)(((r2 * 31 / 3) << 11) | ((g2 * 63 / 3) << 5) | (b2 * 31 / 3));
}

static uint8_t dummy_line[ACTIVE_W] = {0};

void LCDDisplay::Flush() {
  if (!lcdInitialized || !VIDEO::vga.frameBuffer)
    return;

  int srcWidth  = VIDEO::vga.xres;
  int srcHeight = VIDEO::vga.yres;
  int srcX   = (srcWidth  > ACTIVE_W) ? (srcWidth  - ACTIVE_W) / 2 : 0;
  int srcY   = (srcHeight > ACTIVE_H) ? (srcHeight - ACTIVE_H) / 2 : 0;
  int width  = (srcWidth  > ACTIVE_W) ? ACTIVE_W : srcWidth;
  int height = (srcHeight > ACTIVE_H) ? ACTIVE_H : srcHeight;

  tft.startWrite();
  for (int stripY = 0; stripY < height; stripY += STRIP_HEIGHT) {
    int thisStripH = (stripY + STRIP_HEIGHT > height) ? height - stripY : STRIP_HEIGHT;
    for (int line = 0; line < thisStripH; ++line) {
      uint8_t *srcLine = VIDEO::vga.frameBuffer[stripY + line + srcY];
      if (!srcLine) srcLine = dummy_line;
      uint16_t *dst = stripBuffer + line * ACTIVE_W;
      for (int x = 0; x < width; ++x)
        dst[x] = convertPixel(srcLine[(x + srcX) ^ 2]);
      for (int x = width; x < ACTIVE_W; ++x)
        dst[x] = 0;
    }
    tft.setAddrWindow(OFFSET_X, OFFSET_Y + stripY, ACTIVE_W, thisStripH);
    tft.pushColors(stripBuffer, ACTIVE_W * thisStripH, false);
  }
  tft.endWrite();
}

#endif
