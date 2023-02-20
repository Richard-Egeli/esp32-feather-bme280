#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_sleep.h"
#include "i2c.h"
#include "sdkconfig.h"
#include "sensor.h"
#include "wifi.h"

#define TAG "MAIN"
#define CHECK_ERR(func) \
    if (func != ESP_OK) \
        ESP_LOGE(TAG, #func " failed, err: %d", func);

void send_data(char* body) {
    esp_http_client_config_t config = {
        .host   = CONFIG_HTTP_HOST,
        .path   = CONFIG_HTTP_PATH,
        .port   = 443,
        .method = HTTP_METHOD_POST,
#ifdef CONFIG_HTTP_USE_SSL
        .transport_type    = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
#endif
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    if (strnlen(CONFIG_HTTP_TOKEN, 1024) > 0) {
        CHECK_ERR(esp_http_client_set_header(client, "Authorization", "Bearer " CONFIG_HTTP_TOKEN));
    }

    CHECK_ERR(esp_http_client_set_header(client, "Content-Type", "application/json"));
    CHECK_ERR(esp_http_client_set_post_field(client, body, strnlen(body, 2048)));
    CHECK_ERR(esp_http_client_perform(client));
    CHECK_ERR(esp_http_client_cleanup(client));
}

char* read_data(void) {
    sensor_data_t data;
    CHECK_ERR(sensor_read(&data));
    cJSON* body = cJSON_CreateObject();

    cJSON_AddStringToObject(body, "device", CONFIG_DEVICE_ID);
    cJSON_AddNumberToObject(body, "temperature", data.temperature);
    cJSON_AddNumberToObject(body, "pressure", data.pressure);
    cJSON_AddNumberToObject(body, "humidity", data.humidity);

    char* result = cJSON_Print(body);
    cJSON_Minify(result);
    cJSON_Delete(body);

    return result;
}

void app_main(void) {
    CHECK_ERR(nvs_flash_init());
    CHECK_ERR(i2c_init());
    CHECK_ERR(sensor_init());
    CHECK_ERR(wifi_start());

    if (wifi_wait(10000) == ESP_OK) {
        char* body = read_data();
        printf("Body: %s\n", body);

        if (body != NULL) {
            send_data(body);
            free(body);
        } else {
            ESP_LOGE(TAG, "Sensor data body is NULL");
        }
    } else {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
    }

    CHECK_ERR(sensor_deinit());
    CHECK_ERR(i2c_deinit());

    ESP_LOGI(TAG, "Going to sleep for %d seconds", CONFIG_DATA_INTERVAL);
    esp_deep_sleep(1000000LL * CONFIG_DATA_INTERVAL);
}
