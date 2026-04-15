/* Minimal pwm_audio stubs that provide weak symbols. Real component
 * implementations (strong symbols) will override these when linked.
 */

#include <cstdint>
#include <cstddef>

#if defined(__GNUC__)
# define WEAK __attribute__((weak))
#else
# define WEAK
#endif

#if defined(__has_include) && __has_include("pwm_audio.h")
# include "pwm_audio.h"
#else
/* Minimal local type definitions when header is not available. Keep in
 * sync with the real header where necessary.
 */
typedef int esp_err_t;
typedef int ledc_timer_bit_t;
typedef void* TickType_t;
typedef struct { int dummy; } pwm_audio_config_t;
typedef enum { PWM_AUDIO_STATUS_IDLE = 0 } pwm_audio_status_t;
#endif

extern "C" {

esp_err_t WEAK pwm_audio_init(const pwm_audio_config_t *cfg) { (void)cfg; return 0; }
esp_err_t WEAK pwm_audio_deinit(void) { return 0; }
esp_err_t WEAK pwm_audio_start(void) { return 0; }
esp_err_t WEAK pwm_audio_stop(void) { return 0; }
esp_err_t WEAK pwm_audio_write(uint8_t *inbuf, size_t len, size_t *bytes_written, TickType_t ticks_to_wait) {
    (void)inbuf; (void)ticks_to_wait;
    if (bytes_written) *bytes_written = len;
    return 0;
}
esp_err_t WEAK pwm_audio_set_param(int rate, ledc_timer_bit_t bits, int ch) { (void)rate; (void)bits; (void)ch; return 0; }
esp_err_t WEAK pwm_audio_set_sample_rate(int rate) { (void)rate; return 0; }
esp_err_t WEAK pwm_audio_set_volume(int8_t volume) { (void)volume; return 0; }
esp_err_t WEAK pwm_audio_get_volume(int8_t *volume) { if (volume) *volume = 0; return 0; }
esp_err_t WEAK pwm_audio_get_param(int *rate, int *bits, int *ch) { if (rate) *rate = 0; if (bits) *bits = 0; if (ch) *ch = 0; return 0; }
esp_err_t WEAK pwm_audio_get_status(pwm_audio_status_t *status) { if (status) *status = PWM_AUDIO_STATUS_IDLE; return 0; }
uint32_t WEAK pwm_audio_rbstats(void) { return 0; }

}
