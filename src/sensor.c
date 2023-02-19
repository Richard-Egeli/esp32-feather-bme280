#include "sensor.h"

#include "bme280.h"
#include "esp_log.h"
#include "i2c.h"

#define TAG     ("SENSOR")
#define SAMPLES (16)

static struct bme280_dev dev;

static int8_t sensor_bme280_read_data_normal_mode(struct bme280_dev* dev,
                                                  sensor_data_t* data) {
    int8_t rslt;
    uint8_t settings_sel;
    struct bme280_data comp_data;

    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h        = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p        = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t        = BME280_OVERSAMPLING_2X;
    dev->settings.filter       = BME280_FILTER_COEFF_16;
    dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    settings_sel = BME280_OSR_PRESS_SEL;
    settings_sel |= BME280_OSR_TEMP_SEL;
    settings_sel |= BME280_OSR_HUM_SEL;
    settings_sel |= BME280_STANDBY_SEL;
    settings_sel |= BME280_FILTER_SEL;
    rslt = bme280_set_sensor_settings(settings_sel, dev);

    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Unable to set BME280 sensor settings. ERR: %d", rslt);
    }

    rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "Unable to set BME280 sensor mode.");
    }

    /* Delay while the sensor completes a measurement */
    dev->delay_ms(70);

    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);

    data->temperature = comp_data.temperature / 100.0;
    data->pressure    = comp_data.pressure / 10000.0;
    data->humidity    = comp_data.humidity / 1000.0;

    return rslt;
}

esp_err_t sensor_read(sensor_data_t* data) {
    int8_t rslt;

    for (int i = 0; i < SAMPLES; i++) {
        sensor_data_t temp;
        rslt = sensor_bme280_read_data_normal_mode(&dev, &temp);

        if (rslt != BME280_OK) {
            ESP_LOGE(TAG, "failed to read bme280 sensor. ERR: %d", rslt);
            return ESP_FAIL;
        }

        data->temperature += temp.temperature;
        data->pressure += temp.pressure;
        data->humidity += temp.humidity;
    }

    data->temperature /= SAMPLES;
    data->pressure /= SAMPLES;
    data->humidity /= SAMPLES;

    return ESP_OK;
}

esp_err_t sensor_deinit(void) { return gpio_set_level(7, 0); }

esp_err_t sensor_init(void) {
    dev.dev_id   = 0x77;
    dev.intf     = BME280_I2C_INTF;
    dev.read     = i2c_read;
    dev.write    = i2c_write;
    dev.delay_ms = i2c_delay;

    gpio_set_direction(7, GPIO_MODE_OUTPUT);
    gpio_set_level(7, 1);

    vTaskDelay(pdMS_TO_TICKS(3000));

    int8_t rslt = bme280_init(&dev);
    if (rslt != BME280_OK) {
        ESP_LOGE(TAG, "unable to initialize BME280. ERR: %d", rslt);
        return ESP_FAIL;
    }

    return ESP_OK;
}
