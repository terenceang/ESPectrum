#include "LCDDisplay.h"
#ifdef USE_LCD

#include <TFT_eSPI.h>
#include "Video.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xl9535.h"

static TFT_eSPI tft = TFT_eSPI(320, 240);
static uint16_t *lineBuffer = nullptr;
static uint16_t *stripBuffer = nullptr;
static const int LCD_WIDTH = 320;
static const int LCD_HEIGHT = 240;
// Video System Reference active area and strip size
static const int ACTIVE_W = 224;
static const int ACTIVE_H = 288; // documented active height (may be clipped to LCD_HEIGHT)
static const int OFFSET_X = 8;
static const int OFFSET_Y = 16;
static const int STRIP_HEIGHT = 48; // render in 48-pixel strips per documentation
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

  lineBuffer = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * LCD_WIDTH, MALLOC_CAP_INTERNAL);
  if (!lineBuffer) {
    lineBuffer = (uint16_t *)malloc(sizeof(uint16_t) * LCD_WIDTH);
  }
  // Allocate strip buffer (ACTIVE_W * STRIP_HEIGHT) used for zero-copy DMA-friendly transfers
  size_t allocCount = (size_t)ACTIVE_W * STRIP_HEIGHT;
  stripBuffer = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * allocCount, MALLOC_CAP_INTERNAL);
  if (!stripBuffer) {
    stripBuffer = (uint16_t *)malloc(sizeof(uint16_t) * allocCount);
  }

  lcdInitialized = true;
}

uint16_t LCDDisplay::convertPixel(uint8_t pixel) {
  uint8_t color = pixel & 0x3F;
  uint8_t r2 = color & 0x03;
  uint8_t g2 = (color >> 2) & 0x03;
  uint8_t b2 = (color >> 4) & 0x03;

  uint16_t r5 = (r2 * 31) / 3;
  uint16_t g6 = (g2 * 63) / 3;
  uint16_t b5 = (b2 * 31) / 3;

  return (uint16_t)((r5 << 11) | (g6 << 5) | b5);
}

void LCDDisplay::Flush() {
  if (!lcdInitialized)
    return;
  if (!VIDEO::vga.frameBuffer)
    return;

  int srcWidth = VIDEO::vga.xres;
  int srcHeight = VIDEO::vga.yres;
  int srcX = (srcWidth > ACTIVE_W) ? (srcWidth - ACTIVE_W) / 2 : 0;
  int srcY = (srcHeight > ACTIVE_H) ? (srcHeight - ACTIVE_H) / 2 : 0;
  int width = (srcWidth > ACTIVE_W) ? ACTIVE_W : srcWidth;
  // Clip documented active height to the actual LCD height
  int effectiveActiveH = ACTIVE_H;
  if (OFFSET_Y + ACTIVE_H > LCD_HEIGHT) {
    effectiveActiveH = LCD_HEIGHT - OFFSET_Y;
  }
  int height = (srcHeight > effectiveActiveH) ? effectiveActiveH : srcHeight;

  tft.startWrite();
  // Render in STRIP_HEIGHT strips for DMA-friendly transfers (per docs)
  for (int stripY = 0; stripY < height; stripY += STRIP_HEIGHT) {
    int thisStripH = STRIP_HEIGHT;
    if (stripY + thisStripH > height) thisStripH = height - stripY;
    // Fill stripBuffer line by line (ACTIVE_W pixels per line)
    for (int line = 0; line < thisStripH; ++line) {
      int y = stripY + line;
      uint8_t *srcLine = VIDEO::vga.frameBuffer[y + srcY];
      for (int x = 0; x < ACTIVE_W; ++x) {
        uint8_t pixel = 0;
        if (x < width) {
          pixel = srcLine[(x + srcX) ^ 2];
        }
        stripBuffer[line * ACTIVE_W + x] = convertPixel(pixel);
      }
    }
    // Push the strip to the active area (offset by OFFSET_X and OFFSET_Y)
    tft.setAddrWindow(OFFSET_X, OFFSET_Y + stripY, ACTIVE_W, thisStripH);
    tft.pushColors(stripBuffer, ACTIVE_W * thisStripH, false);
  }
  tft.endWrite();
}

#endif
