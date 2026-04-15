/* Minimal pwm_audio stubs to satisfy linker when pwm_audio component is disabled.
 * These functions intentionally do nothing and return success / neutral values.
 */

#include "pwm_audio.h"

extern "C" {

esp_err_t pwm_audio_init(const pwm_audio_config_t *cfg) { (void)cfg; return ESP_OK; }
esp_err_t pwm_audio_deinit(void) { return ESP_OK; }
esp_err_t pwm_audio_start(void) { return ESP_OK; }
esp_err_t pwm_audio_stop(void) { return ESP_OK; }
esp_err_t pwm_audio_write(uint8_t *inbuf, size_t len, size_t *bytes_written, TickType_t ticks_to_wait) {
    (void)inbuf; (void)ticks_to_wait;
    if (bytes_written) *bytes_written = len;
    return ESP_OK;
}
esp_err_t pwm_audio_set_param(int rate, ledc_timer_bit_t bits, int ch) { (void)rate; (void)bits; (void)ch; return ESP_OK; }
esp_err_t pwm_audio_set_sample_rate(int rate) { (void)rate; return ESP_OK; }
esp_err_t pwm_audio_set_volume(int8_t volume) { (void)volume; return ESP_OK; }
esp_err_t pwm_audio_get_volume(int8_t *volume) { if (volume) *volume = 0; return ESP_OK; }
esp_err_t pwm_audio_get_param(int *rate, int *bits, int *ch) { if (rate) *rate = 0; if (bits) *bits = 0; if (ch) *ch = 0; return ESP_OK; }
esp_err_t pwm_audio_get_status(pwm_audio_status_t *status) { if (status) *status = PWM_AUDIO_STATUS_IDLE; return ESP_OK; }
uint32_t pwm_audio_rbstats(void) { return 0; }

}
