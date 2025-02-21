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
// HTML page to display the chart
const char *html_page = 
    "<!DOCTYPE html>"
    "<html>"
    "<head><title>ESP32 WEB SERVER</title>"
    "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>"
    "<style>"
        ".switch {"
            "position: relative;"
            "display: inline-block;"
            "width: 60px;"
            "height: 34px;"
        "}"
        ".switch input {"
            "opacity: 0;"
            "width: 0;"
            "height: 0;"
        "}"
        ".slider {"
            "position: absolute;"
            "cursor: pointer;"
            "top: 0;"
            "left: 0;"
            "right: 0;"
            "bottom: 0;"
            "background-color: #ccc;"
            "transition: 0.4s;"
            "border-radius: 34px;"
        "}"
        ".slider:before {"
            "position: absolute;"
            "content: \"\";"
            "height: 26px;"
            "width: 26px;"
            "left: 4px;"
            "bottom: 4px;"
            "background-color: white;"
            "transition: 0.4s;"
            "border-radius: 50%;"
        "}"
        "input:checked + .slider {"
            "background-color: #2196F3;"
        "}"
        "input:checked + .slider:before {"
            "transform: translateX(26px);"
        "}"
    "</style>"
    "</head>"
    "<body>"
    "<h2>Real-time Data Charts</h2>"
    "<h3>Line Charts</h3>"
    "<canvas id=\"lineChart1\" width=\"400\" height=\"200\"></canvas><br>"
    "<canvas id=\"lineChart2\" width=\"400\" height=\"200\"></canvas><br>"
    "<canvas id=\"lineChart3\" width=\"400\" height=\"200\"></canvas>"
    "<h3>Pie Charts</h3>"
    "<canvas id=\"pieChart1\" width=\"400\" height=\"200\"></canvas><br>"
    "<canvas id=\"pieChart2\" width=\"400\" height=\"200\"></canvas><br>"
    "<canvas id=\"pieChart3\" width=\"400\" height=\"200\"></canvas>"
    "<h4>Detection Data</h4>"
    "<p id=\"early_blight\">Early Blight Count: 0</p>"
    "<p id=\"mold_leaf\">Mold Leaf Count: 0</p>"
    "<p id=\"tomato_healthy\">Tomato Healthy Count: 0</p>"
    "<h2>Switch Button Example</h2>"
    "<label class=\"switch\">"
        "<input type=\"checkbox\" id=\"toggleSwitch\">"
        "<span class=\"slider\"></span>"
    "</label>"
    "<p id=\"status\">Switch is off</p>"
    "<script>"
        "let a_switch = \"off\";"
        "const toggleSwitch = document.getElementById(\"toggleSwitch\");"
        "const statusText = document.getElementById(\"status\");"
        "toggleSwitch.addEventListener(\"change\", function () {"
            "if (toggleSwitch.checked) {"
                "a_switch = \"on\";"
                "statusText.textContent = \"Switch is on\";"
            "} else {"
                "a_switch = \"off\";"
                "statusText.textContent = \"Switch is off\";"
            "}"
            "console.log(\"Switch status:\", a_switch);"
        "});"
        "const lineCtx1 = document.getElementById('lineChart1').getContext('2d');"
        "const lineCtx2 = document.getElementById('lineChart2').getContext('2d');"
        "const lineCtx3 = document.getElementById('lineChart3').getContext('2d');"
        "const pieCtx1 = document.getElementById('pieChart1').getContext('2d');"
        "const pieCtx2 = document.getElementById('pieChart2').getContext('2d');"
        "const pieCtx3 = document.getElementById('pieChart3').getContext('2d');"
        "const lineChart1 = new Chart(lineCtx1, {"
            "type: 'line',"
            "data: {"
                "labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],"
                "datasets: [{"
                    "label: 'Temperature',"
                    "data: [],"
                    "borderColor: 'rgba(75, 192, 192, 1)',"
                    "fill: false"
                "}]"
            "},"
            "options: {"
                "responsive: true,"
                "scales: {"
                    "x: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' } },"
                    "y: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' }, beginAtZero: true }"
                "}"
            "}"
        "});"
        "const lineChart2 = new Chart(lineCtx2, {"
            "type: 'line',"
            "data: {"
                "labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],"
                "datasets: [{"
                    "label: 'Humidity',"
                    "data: [],"
                    "borderColor: 'rgba(153, 102, 255, 1)',"
                    "fill: false"
                "}]"
            "},"
            "options: {"
                "responsive: true,"
                "scales: {"
                    "x: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' } },"
                    "y: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' }, beginAtZero: true }"
                "}"
            "}"
        "});"
        "const lineChart3 = new Chart(lineCtx3, {"
            "type: 'line',"
            "data: {"
                "labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],"
                "datasets: [{"
                    "label: 'Soil Moisture',"
                    "data: [],"
                    "borderColor: 'rgba(255, 159, 64, 1)',"
                    "fill: false"
                "}]"
            "},"
            "options: {"
                "responsive: true,"
                "scales: {"
                    "x: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' } },"
                    "y: { grid: { display: true, color: 'rgba(200, 200, 200, 0.8)' }, beginAtZero: true }"
                "}"
            "}"
        "});"
        "function updateChartData() {"
            "fetch('/data').then(response => response.json()).then(data => {"
                "lineChart1.data.datasets[0].data = data.type1;"
                "lineChart2.data.datasets[0].data = data.type2;"
                "lineChart3.data.datasets[0].data = data.type3;"
                "document.getElementById('early_blight').textContent = 'Early Blight Count: ' + data.early_blight;"
                "document.getElementById('mold_leaf').textContent = 'Mold Leaf Count: ' + data.mold_leaf;"
                "document.getElementById('tomato_healthy').textContent = 'Tomato Healthy Count: ' + data.tomato_healthy;"
                "lineChart1.update();"
                "lineChart2.update();"
                "lineChart3.update();"
            "}).catch(error => console.error('Error fetching data:', error));"
        "}"
        "setInterval(updateChartData, 1000);"
    "</script>"
    "</body>"
    "</html>";
    

esp_err_t html_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

// Register web server handlers
static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_html = { .uri = "/", .method = HTTP_GET, .handler = html_handler };
        httpd_uri_t uri_data = { .uri = "/data", .method = HTTP_GET, .handler = data_handler };

        httpd_register_uri_handler(server, &uri_html);
        httpd_register_uri_handler(server, &uri_data);

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