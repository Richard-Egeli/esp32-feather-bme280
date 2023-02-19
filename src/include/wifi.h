#ifndef SRC_WIFI_H_
#define SRC_WIFI_H_

#include <stdio.h>

#include "esp_err.h"

esp_err_t wifi_start(void);
esp_err_t wifi_wait(const uint16_t timeout_ms);

#endif  // SRC_WIFI_H_
