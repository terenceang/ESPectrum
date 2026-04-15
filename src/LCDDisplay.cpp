#include "Video.h"   // must come first — defines USE_LCD
#include "LCDDisplay.h"

#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xl9535.h"
#include "esp_lcd_panel_io.h"
#include "driver/spi_master.h"
#include "rom/ets_sys.h"
#include "OSDMain.h"
#include <string.h>

static const int LCD_WIDTH    = 320;
static const int LCD_HEIGHT   = 240;
static const int OFFSET_X     = 0;
static const int OFFSET_Y     = 0;
static const int ACTIVE_W     = LCD_WIDTH;
static const int ACTIVE_H     = LCD_HEIGHT;
static const int STRIP_HEIGHT = 48;

static const int PIN_MOSI = 21;
static const int PIN_SCLK = 12;
static const int PIN_CS   = 14;
static const int PIN_DC   = 13;

static const spi_host_device_t LCD_SPI_HOST = SPI3_HOST;

static esp_lcd_panel_io_handle_t io_handle = nullptr;
static uint16_t *stripBuffer = nullptr;
static bool lcdInitialized = false;

static void lcd_cmd(uint8_t cmd, const void *data = nullptr, size_t len = 0) {
    esp_lcd_panel_io_tx_param(io_handle, cmd, data, len);
}

static void ili9341_init() {
    const uint8_t p_cb[] = {0x39,0x2C,0x00,0x34,0x02};
    lcd_cmd(0xCB, p_cb, sizeof(p_cb));
    const uint8_t p_cf[] = {0x00,0xC1,0x30};
    lcd_cmd(0xCF, p_cf, sizeof(p_cf));
    const uint8_t p_e8[] = {0x85,0x00,0x78};
    lcd_cmd(0xE8, p_e8, sizeof(p_e8));
    const uint8_t p_ea[] = {0x00,0x00};
    lcd_cmd(0xEA, p_ea, sizeof(p_ea));
    const uint8_t p_ed[] = {0x64,0x03,0x12,0x81};
    lcd_cmd(0xED, p_ed, sizeof(p_ed));
    const uint8_t p_f7[] = {0x20};
    lcd_cmd(0xF7, p_f7, sizeof(p_f7));
    const uint8_t p_c0[] = {0x23};
    lcd_cmd(0xC0, p_c0, sizeof(p_c0));           // Power Control 1
    const uint8_t p_c1[] = {0x10};
    lcd_cmd(0xC1, p_c1, sizeof(p_c1));           // Power Control 2
    const uint8_t p_c5[] = {0x3E,0x28};
    lcd_cmd(0xC5, p_c5, sizeof(p_c5));           // VCom Control 1
    const uint8_t p_c7[] = {0x86};
    lcd_cmd(0xC7, p_c7, sizeof(p_c7));           // VCom Control 2
    const uint8_t p_36[] = {0xE8};
    lcd_cmd(0x36, p_36, sizeof(p_36));           // MADCTL: landscape (MV+BGR, MY=1) + MX (horizontal flip)
    const uint8_t p_3a[] = {0x55};
    lcd_cmd(0x3A, p_3a, sizeof(p_3a));           // 16-bit colour
    const uint8_t p_b1[] = {0x00,0x18};
    lcd_cmd(0xB1, p_b1, sizeof(p_b1));           // Frame Rate: 79Hz
    const uint8_t p_b6[] = {0x08,0x82,0x27};
    lcd_cmd(0xB6, p_b6, sizeof(p_b6));           // Display Function Control
    const uint8_t p_f2[] = {0x00};
    lcd_cmd(0xF2, p_f2, sizeof(p_f2));           // 3Gamma off
    const uint8_t p_26[] = {0x01};
    lcd_cmd(0x26, p_26, sizeof(p_26));           // Gamma Curve 1
    const uint8_t p_e0[] = {0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,
                              0x37,0x07,0x10,0x03,0x0E,0x09,0x00};
    lcd_cmd(0xE0, p_e0, sizeof(p_e0));           // Positive Gamma
    const uint8_t p_e1[] = {0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,
                              0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
    lcd_cmd(0xE1, p_e1, sizeof(p_e1));           // Negative Gamma
    lcd_cmd(0x11);                               // Sleep Out
    ets_delay_us(120000);                        // 120ms
    lcd_cmd(0x29);                               // Display On
}

static void set_addr_window(int x, int y, int w, int h) {
    const int x2 = x + w - 1;
    const int y2 = y + h - 1;
    const uint8_t col[] = {(uint8_t)(x >> 8), (uint8_t)x,
                            (uint8_t)(x2 >> 8), (uint8_t)x2};
    const uint8_t row[] = {(uint8_t)(y >> 8), (uint8_t)y,
                            (uint8_t)(y2 >> 8), (uint8_t)y2};
    esp_lcd_panel_io_tx_param(io_handle, 0x2A, col, sizeof(col));
    esp_lcd_panel_io_tx_param(io_handle, 0x2B, row, sizeof(row));
}

// Returns RGB565 big-endian — ILI9341 expects MSB first over SPI.
static uint16_t convertPixel(uint8_t pixel) {
    uint8_t color = pixel & 0x3F;
    uint8_t r2 = color & 0x03;
    uint8_t g2 = (color >> 2) & 0x03;
    uint8_t b2 = (color >> 4) & 0x03;
    uint16_t rgb = (uint16_t)(((r2 * 31 / 3) << 11) | ((g2 * 63 / 3) << 5) | (b2 * 31 / 3));
    return __builtin_bswap16(rgb);
}

static uint8_t dummy_line[LCD_WIDTH] = {0};

void LCDDisplay::Init() {
    if (lcdInitialized) return;

    printf("[LCD] Init: SPI%d MOSI=%d SCLK=%d CS=%d DC=%d @ 40MHz  buf=%dx%d\n",
           (int)LCD_SPI_HOST, PIN_MOSI, PIN_SCLK, PIN_CS, PIN_DC, LCD_WIDTH, LCD_HEIGHT);

    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = PIN_MOSI;
    bus_cfg.miso_io_num = -1;
    bus_cfg.sclk_io_num = PIN_SCLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = ACTIVE_W * STRIP_HEIGHT * sizeof(uint16_t);
    spi_bus_initialize(LCD_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

    esp_lcd_panel_io_spi_config_t io_cfg = {};
    io_cfg.dc_gpio_num      = PIN_DC;
    io_cfg.cs_gpio_num      = PIN_CS;
    io_cfg.pclk_hz          = 40 * 1000 * 1000;
    io_cfg.lcd_cmd_bits     = 8;
    io_cfg.lcd_param_bits   = 8;
    io_cfg.spi_mode         = 0;
    io_cfg.trans_queue_depth = 10;
    // on_color_trans_done left NULL → esp_lcd_panel_io_tx_color blocks until done
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_cfg, &io_handle);

    XL9535::init();
    XL9535::setBacklight(true);

    ili9341_init();

    // Allocate frame line buffers so Video.cpp can render into them.
    // One pointer per scanline; try PSRAM first to preserve internal DRAM.
    {
        size_t fbSize = (size_t)LCD_WIDTH * LCD_HEIGHT;
        uint8_t *fb = (uint8_t *)heap_caps_malloc(fbSize, MALLOC_CAP_SPIRAM);
        if (!fb) fb = (uint8_t *)malloc(fbSize);
        if (fb) {
            memset(fb, 0, fbSize);
            for (int i = 0; i < LCD_HEIGHT; i++)
                VIDEO::vga.frameBuffer[i] = fb + i * LCD_WIDTH;
            printf("[LCD] frameBuffer: %p (%zu B, %s)\n", (void *)fb, fbSize,
                   esp_ptr_external_ram(fb) ? "PSRAM" : "DRAM");
        } else {
            printf("[LCD] ERROR: frameBuffer alloc failed!\n");
        }
    }

    size_t allocCount = (size_t)ACTIVE_W * STRIP_HEIGHT;
    stripBuffer = (uint16_t *)heap_caps_malloc(sizeof(uint16_t) * allocCount,
                                                MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!stripBuffer)
        stripBuffer = (uint16_t *)malloc(sizeof(uint16_t) * allocCount);

    printf("[LCD] Init done: stripBuffer=%p (%zu bytes)  backlight=ON\n",
           (void *)stripBuffer, allocCount * sizeof(uint16_t));

    lcdInitialized = true;
}

void LCDDisplay::Flush() {
    if (!lcdInitialized || !stripBuffer || !VIDEO::vga.frameBuffer[0]) return;

    const int srcWidth  = OSD::scrW;
    const int srcHeight = OSD::scrH;
    const int srcX   = (srcWidth  > ACTIVE_W) ? (srcWidth  - ACTIVE_W) / 2 : 0;
    const int srcY   = (srcHeight > ACTIVE_H) ? (srcHeight - ACTIVE_H) / 2 : 0;
    const int width  = (srcWidth  > ACTIVE_W) ? ACTIVE_W : srcWidth;
    const int height = (srcHeight > ACTIVE_H) ? ACTIVE_H : srcHeight;
    // Render and transfer the visible region in vertical "strips" so the
    // transfer buffer fits in RAM/DMA limits. Each strip is up to STRIP_HEIGHT
    // scanlines tall.
    for (int stripY = 0; stripY < height; stripY += STRIP_HEIGHT) {
        // Height of this strip (may be smaller at bottom edge).
        const int thisStripH = (stripY + STRIP_HEIGHT > height) ? height - stripY : STRIP_HEIGHT;

        // Convert each source scanline in the strip into the 16-bit RGB565
        // stripBuffer expected by the ILI9341. Video framebuffer stores one
        // byte per pixel; convertPixel() maps that to big-endian RGB565.
        for (int line = 0; line < thisStripH; ++line) {
            uint8_t *srcLine = (uint8_t *)VIDEO::vga.frameBuffer[stripY + line + srcY];
            if (!srcLine) srcLine = dummy_line; // fallback to a blank line
            uint16_t *dst = stripBuffer + line * ACTIVE_W;

            // Copy and convert visible pixels. The source X coordinate is
            // offset by srcX and XOR'd with 2 (hardware/format alignment).
            for (int x = 0; x < width; ++x)
                dst[x] = convertPixel(srcLine[(x + srcX) ^ 2]);

            // Fill any remaining pixels on the scanline (right side) with
            // black (0) so the full ACTIVE_W width is always transmitted.
            for (int x = width; x < ACTIVE_W; ++x)
                dst[x] = 0;
        }

        // Tell the LCD which rectangle we're about to update and send the
        // strip's pixel data. Command 0x2C is the memory write (RAMWR).
        set_addr_window(OFFSET_X, OFFSET_Y + stripY, ACTIVE_W, thisStripH);
        esp_lcd_panel_io_tx_color(io_handle, 0x2C, stripBuffer,
                                   ACTIVE_W * thisStripH * sizeof(uint16_t));
    }
}
