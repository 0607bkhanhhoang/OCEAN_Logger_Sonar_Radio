#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#define WIFI_SSID "hujjj"
#define WIFI_PASS "12345678"

static const char *TAG = "HTTP_SERVER";

// Generate random number function
int generate_random(int min, int max) {
    return min + rand();
}

static esp_err_t data_handler(httpd_req_t *req) {
    char response[1024]; // Increased buffer size

    // Generate all random numbers before snprintf to avoid errors
    int type1[10], type2[10], type3[10];
    for (int i = 0; i < 10; i++) {
        type1[i] = generate_random(1, 100);
        type2[i] = generate_random(1, 100);
        type3[i] = generate_random(1, 100);
    }
    int early_blight = generate_random(1, 50);
    int mold_leaf = generate_random(1, 50);
    int tomato_healthy = generate_random(1, 50);

    // Now use the generated values safely
    int bytes_written = snprintf(response, sizeof(response),
        "{\"type1\": [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d],"
        " \"type2\": [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d],"
        " \"type3\": [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d],"
        " \"early_blight\": %d,"
        " \"mold_leaf\": %d,"
        " \"tomato_healthy\": %d}",
        type1[0], type1[1], type1[2], type1[3], type1[4], type1[5], type1[6], type1[7], type1[8], type1[9],
        type2[0], type2[1], type2[2], type2[3], type2[4], type2[5], type2[6], type2[7], type2[8], type2[9],
        type3[0], type3[1], type3[2], type3[3], type3[4], type3[5], type3[6], type3[7], type3[8], type3[9],
        early_blight, mold_leaf, tomato_healthy
    );

    if (bytes_written >= sizeof(response)) {
        ESP_LOGE("DATA_HANDLER", "JSON response truncated!");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// Register web server handlers
static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t data_uri = {
            .uri = "/data",
            .method = HTTP_GET,
            .handler = data_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &data_uri);
        ESP_LOGI(TAG, "Web server started");
    }
    return server;
}

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    static httpd_handle_t server = NULL;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (server) {
            httpd_stop(server);
            server = NULL;
        }
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if (!server) {
            server = start_webserver();
        }
    }
}

// Initialize Wi-Fi
static void wifi_init_sta(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// Main function
void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init_sta();
}