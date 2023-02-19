#ifndef SRC_INCLUDE_SENSOR_H_
#define SRC_INCLUDE_SENSOR_H_

#include "esp_err.h"

typedef struct sensor_data_struct {
    float temperature;
    float pressure;
    float humidity;
} sensor_data_t;

esp_err_t sensor_init(void);
esp_err_t sensor_deinit(void);

esp_err_t sensor_read(sensor_data_t* data);

#endif  // SRC_INCLUDE_SENSOR_H_
