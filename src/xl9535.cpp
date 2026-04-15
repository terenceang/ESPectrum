#include "../include/xl9535.h"
#include "driver/i2c.h"
#include <esp_log.h>

static const char *TAG = "xl9535";

// I2C configuration for UniHiker K10 (per hardware reference)
#define XL9535_I2C_PORT I2C_NUM_0
#define XL9535_SDA_IO 47
#define XL9535_SCL_IO 48
#define XL9535_FREQ_HZ 400000

#define XL9535_ADDRESS 0x20

// XL9535 register map (K10 docs)
#define XL9535_REG_INPUT_PORT_0  0x00
#define XL9535_REG_INPUT_PORT_1  0x01
#define XL9535_REG_OUTPUT_PORT_0 0x02
#define XL9535_REG_OUTPUT_PORT_1 0x03
#define XL9535_REG_CONFIG_PORT_0 0x06
#define XL9535_REG_CONFIG_PORT_1 0x07

static esp_err_t xl9535_write_reg(uint8_t reg, uint8_t val) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (XL9535_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  i2c_master_write_byte(cmd, val, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(XL9535_I2C_PORT, cmd, pdMS_TO_TICKS(100));
  i2c_cmd_link_delete(cmd);
  return ret;
}

static esp_err_t xl9535_read_reg(uint8_t reg, uint8_t *out) {
  if (!out) return ESP_ERR_INVALID_ARG;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (XL9535_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (XL9535_ADDRESS << 1) | I2C_MASTER_READ, true);
  i2c_master_read_byte(cmd, out, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(XL9535_I2C_PORT, cmd, pdMS_TO_TICKS(100));
  i2c_cmd_link_delete(cmd);
  return ret;
}

namespace XL9535 {

void init() {
  // Configure I2C driver
  i2c_config_t conf = {};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = (gpio_num_t)XL9535_SDA_IO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = (gpio_num_t)XL9535_SCL_IO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = XL9535_FREQ_HZ;
  i2c_param_config(XL9535_I2C_PORT, &conf);
  i2c_driver_install(XL9535_I2C_PORT, conf.mode, 0, 0, 0);

  // Configure port directions: make P0 bit0 an output (0 = output, 1 = input)
  // Default: configure other pins as inputs to avoid interfering with other hardware.
  uint8_t cfg0 = 0xFE; // bit0 = 0 (output), others = 1 (inputs)
  if (xl9535_write_reg(XL9535_REG_CONFIG_PORT_0, cfg0) != ESP_OK) {
    ESP_LOGW(TAG, "Failed to write CONFIG_PORT_0");
  }

  // Ensure output port 0 has backlight off initially
  uint8_t out0 = 0x00;
  if (xl9535_write_reg(XL9535_REG_OUTPUT_PORT_0, out0) != ESP_OK) {
    ESP_LOGW(TAG, "Failed to write OUTPUT_PORT_0");
  }
}

void setBacklight(bool on) {
  // Read current output port 0, set/clear bit0
  uint8_t cur = 0;
  if (xl9535_read_reg(XL9535_REG_OUTPUT_PORT_0, &cur) != ESP_OK) {
    // on read failure, attempt a blind write
    uint8_t val = on ? 0x01 : 0x00;
    xl9535_write_reg(XL9535_REG_OUTPUT_PORT_0, val);
    return;
  }
  if (on) cur |= 0x01; else cur &= ~0x01;
  xl9535_write_reg(XL9535_REG_OUTPUT_PORT_0, cur);
}

} // namespace XL9535
