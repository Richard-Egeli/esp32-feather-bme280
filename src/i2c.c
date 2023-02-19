#include "i2c.h"

static const char *TAG = "i2c";

esp_err_t i2c_deinit() { return i2c_driver_delete(I2C_DEFAULT_PORT); }

void i2c_delay(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

esp_err_t i2c_read(uint8_t addr,
                   uint8_t reg_addr,
                   uint8_t *buffer,
                   uint16_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);

    if (length > 1) {
        i2c_master_read(cmd, buffer, length - 1, I2C_MASTER_ACK);
    }

    i2c_master_read_byte(cmd, buffer + length - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_DEFAULT_PORT, cmd, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(cmd);

    return ESP_OK;
}

esp_err_t i2c_write(uint8_t addr,
                    uint8_t reg_addr,
                    uint8_t *buffer,
                    uint16_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);

    i2c_master_write(cmd, buffer, length, true);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_DEFAULT_PORT, cmd, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(cmd);
    return 0;
}

esp_err_t i2c_init(void) {
    i2c_config_t config = I2C_DEFAULT_CONFIG;
    esp_err_t err       = i2c_param_config(I2C_DEFAULT_PORT, &config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to set i2c param config");
        return err;
    }

    err = i2c_driver_install(I2C_DEFAULT_PORT, config.mode, 0, 0, 0);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to install i2c driver");
    }

    return err;
}
