// Minimal compatibility header for esp chip/efuse/flash info used by OSD
#ifndef ESP_COMPAT_H
#define ESP_COMPAT_H
#include <stdint.h>

// Prefer including the real esp headers when available to avoid duplicate typedefs.
#if defined(__has_include)
# if __has_include(<esp_chip_info.h>)
#  include <esp_chip_info.h>
# else
// Fallback definition when esp_chip_info.h is not available
typedef struct {
    int model;
    int cores;
    uint32_t features;
    int revision;
} esp_chip_info_t;

void esp_chip_info(esp_chip_info_t* info);
# endif
#else
// No __has_include: provide a conservative fallback
typedef struct {
    int model;
    int cores;
    uint32_t features;
    int revision;
} esp_chip_info_t;

void esp_chip_info(esp_chip_info_t* info);
#endif

// efuse / spi flash helpers
#if defined(__has_include) && __has_include(<esp_efuse.h>)
# include <esp_efuse.h>
#endif

// GPIO driver (types and helpers) — include when available so ZXKeyb and others can use gpio_* APIs
#if defined(__has_include) && __has_include(<driver/gpio.h>)
# include <driver/gpio.h>
#endif

// esp timer (esp_timer_get_time) used by Video interrupt timing
#if defined(__has_include) && __has_include(<esp_timer.h>)
# include <esp_timer.h>
#endif

// Provide prototype for esp_timer_get_time if header not available
#if !defined(__has_include) || !__has_include(<esp_timer.h>)
extern "C" int64_t esp_timer_get_time(void);
#endif

#if defined(__has_include) && __has_include(<spi_flash_mmap.h>)
# include <spi_flash_mmap.h>
#else
// Fallback prototypes
uint32_t spi_flash_get_chip_size(void);
#endif

// If efuse package version function is missing, declare a stub prototype.
#if defined(__has_include) && __has_include(<esp_efuse.h>)
// real header included above
#else
uint32_t esp_efuse_get_pkg_ver(void);
#endif

// EFUSE package version constants used by OSD (safe defaults)
#ifndef EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6 1
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5 2
#define EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 3
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 4
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 5
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302 6
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3 7
#endif

// Feature flags
#ifndef CHIP_FEATURE_EMB_FLASH
#define CHIP_FEATURE_EMB_FLASH 0x01u
#endif

// ASCII helpers
#ifndef ASCII_SPC
#define ASCII_SPC 32
#endif
#ifndef ASCII_TAB
#define ASCII_TAB 9
#endif

// RTC WDT stubs/prototypes (provide minimal API expected by OSD)
#if defined(__cplusplus)
extern "C" {
#endif

// Minimal esp_err_t for compatibility
typedef int esp_err_t;

void rtc_wdt_enable(void);
void rtc_wdt_protect_on(void);
void rtc_wdt_protect_off(void);
void rtc_wdt_set_stage(int stage, int action);
void rtc_wdt_set_time(int stage, int time_ms);

// Common constants used by code
#ifndef RTC_WDT_STAGE0
#define RTC_WDT_STAGE0 0
#endif
#ifndef RTC_WDT_STAGE_ACTION_RESET_RTC
#define RTC_WDT_STAGE_ACTION_RESET_RTC 1
#endif

// Ensure spi_flash_get_chip_size is available to OSD code
uint32_t spi_flash_get_chip_size(void);

#if defined(__cplusplus)
}
#endif

#endif // ESP_COMPAT_H
