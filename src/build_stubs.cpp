#include <string>
#include <cstdint>

// Pull in esp compatibility helpers which include real headers when available
#include "../include/esp_compat.h"
// Provide weak no-op rtc watchdog stubs so builds without the full ESP
// implementation still link. Real implementations (strong symbols) will
// override these weak definitions when available.
#if defined(__GNUC__)
# define WEAK __attribute__((weak))
#else
# define WEAK
#endif

extern "C" {
void WEAK rtc_wdt_enable() {}
void WEAK rtc_wdt_protect_on() {}
void WEAK rtc_wdt_protect_off() {}
void WEAK rtc_wdt_set_stage(int /*stage*/, int /*action*/) {}
void WEAK rtc_wdt_set_time(int /*stage*/, int /*time_ms*/) {}
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

// Provide weak firmware/ROM update stubs and a minimal flash stub. These are
// weak so real platform implementations override them when present.
esp_err_t WEAK stub_updateFirmware(const std::string & /*firmware*/) {
    return ESP_OK;
}

esp_err_t WEAK stub_updateROM(const std::string & /*romfile*/, int /*slot*/) {
    return ESP_OK;
}

// Provide minimal flash stub used by OSD when real esp headers are not present.
// Note: do NOT provide esp_efuse_get_pkg_ver here to avoid duplicate symbols
extern "C" uint32_t WEAK spi_flash_get_chip_size(void) {
    // Default to 4MB for stubs
    return 4 * 1024 * 1024;
}
