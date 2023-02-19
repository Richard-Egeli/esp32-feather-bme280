#ifndef SRC_INCLUDE_I2C_H_
#define SRC_INCLUDE_I2C_H_

#include <stdio.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define I2C_DEFAULT_PORT       (0)
#define I2C_DEFAULT_SDA_IO_NUM (3)
#define I2C_DEFAULT_SCL_IO_NUM (4)
#define I2C_DEFAULT_CLK_SPEED  (100000)
#define I2C_DEFAULT_CONFIG                          \
    {                                               \
        .mode             = I2C_MODE_MASTER,        \
        .sda_io_num       = I2C_DEFAULT_SDA_IO_NUM, \
        .scl_io_num       = I2C_DEFAULT_SCL_IO_NUM, \
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,     \
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,     \
        .master.clk_speed = I2C_DEFAULT_CLK_SPEED,  \
    };

typedef enum i2c_ports_enum {
    I2C_PORT_0_INITIALIZED = BIT0,
    I2C_PORT_1_INITIALIZED = BIT5,
} i2c_ports_t;

esp_err_t i2c_init(void);
esp_err_t i2c_deinit(void);
esp_err_t i2c_read(uint8_t addr,
                   uint8_t reg_addr,
                   uint8_t *buffer,
                   uint16_t length);

esp_err_t i2c_write(uint8_t addr,
                    uint8_t reg_addr,
                    uint8_t *buffer,
                    uint16_t length);

void i2c_delay(uint32_t ms);

#endif  // SRC_INCLUDE_I2C_H_
