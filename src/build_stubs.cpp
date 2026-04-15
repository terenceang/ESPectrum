#include <string>
#include <cstdint>

// Pull in esp compatibility helpers which include real headers when available
#include "../include/esp_compat.h"

extern "C" {
// Minimal no-op rtc watchdog stubs used by OSD. Real implementations are platform-specific.
void rtc_wdt_enable() {}
void rtc_wdt_protect_on() {}
void rtc_wdt_protect_off() {}
void rtc_wdt_set_stage(int /*stage*/, int /*action*/) {}
void rtc_wdt_set_time(int /*stage*/, int /*time_ms*/) {}
}

// Provide an esp_chip_info implementation only when the real header is not available.
#if !defined(__has_include) || !__has_include(<esp_chip_info.h>)
extern "C" void esp_chip_info(esp_chip_info_t* info) {
    if (info) {
        info->model = 0;
        info->cores = 1;
        info->features = 0;
        info->revision = 0;
    }
}
#endif

// Firmware/ROM update stubs to skip OTA/ROM flashing in stubbed builds.
#if defined(__has_include) && __has_include("esp_err.h")
# include "esp_err.h"
#endif

#ifndef ESP_OK
# define ESP_OK 0
typedef int esp_err_t;
#endif

esp_err_t stub_updateFirmware(const std::string & /*firmware*/) {
    return ESP_OK;
}

esp_err_t stub_updateROM(const std::string & /*romfile*/, int /*slot*/) {
    return ESP_OK;
}

// Provide minimal flash stub used by OSD when real esp headers are not present.
// Note: do NOT provide esp_efuse_get_pkg_ver here to avoid duplicate symbols
extern "C" uint32_t spi_flash_get_chip_size(void) {
    // Default to 4MB for stubs
    return 4 * 1024 * 1024;
}
