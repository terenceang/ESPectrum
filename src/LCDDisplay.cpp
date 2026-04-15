#include "LCDDisplay.h"
#ifdef USE_LCD

#include <TFT_eSPI.h>
#include "Video.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TFT_eSPI tft = TFT_eSPI(240, 320);
static uint16_t *lineBuffer = nullptr;
static const int LCD_WIDTH = 320;
static const int LCD_HEIGHT = 240;
static bool lcdInitialized = false;

void LCDDisplay::Init() {
  if (lcdInitialized)
    return;

  tft.begin();
  tft.setRotation(1); // landscape orientation for 320x240 output
  tft.fillScreen(TFT_BLACK);

  lineBuffer = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * LCD_WIDTH, MALLOC_CAP_INTERNAL);
  if (!lineBuffer) {
    lineBuffer = (uint16_t *)malloc(sizeof(uint16_t) * LCD_WIDTH);
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
  int srcX = srcWidth > LCD_WIDTH ? (srcWidth - LCD_WIDTH) / 2 : 0;
  int srcY = srcHeight > LCD_HEIGHT ? (srcHeight - LCD_HEIGHT) / 2 : 0;
  int width = srcWidth > LCD_WIDTH ? LCD_WIDTH : srcWidth;
  int height = srcHeight > LCD_HEIGHT ? LCD_HEIGHT : srcHeight;

  tft.startWrite();
  for (int y = 0; y < LCD_HEIGHT; y++) {
    if (y < height) {
      uint8_t *srcLine = VIDEO::vga.frameBuffer[y + srcY];
      for (int x = 0; x < LCD_WIDTH; x++) {
        uint8_t pixel = 0;
        if (x < width) {
          pixel = srcLine[(x + srcX) ^ 2];
        }
        lineBuffer[x] = convertPixel(pixel);
      }
    } else {
      for (int x = 0; x < LCD_WIDTH; x++) {
        lineBuffer[x] = 0;
      }
    }
    tft.setAddrWindow(0, y, LCD_WIDTH, 1);
    tft.pushColors(lineBuffer, LCD_WIDTH, false);
  }
  tft.endWrite();
}

#endif
