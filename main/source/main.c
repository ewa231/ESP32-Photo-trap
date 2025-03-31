#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "camera_config.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_wifi.h"

static const char *TAG = "ESP32-CAM";

// Camera Initialization
esp_err_t init_camera() 
{
    camera_config_t config =
    {
		.ledc_channel = LEDC_CHANNEL_0,
		.ledc_timer = LEDC_TIMER_0,
		.pin_d0 = CAM_PIN_D0,
		.pin_d1 = CAM_PIN_D1,
		.pin_d2 = CAM_PIN_D2,
		.pin_d3 = CAM_PIN_D3,
		.pin_d4 = CAM_PIN_D4,
		.pin_d5 = CAM_PIN_D5,
		.pin_d6 = CAM_PIN_D6,
		.pin_d7 = CAM_PIN_D7,
		.pin_xclk = CAM_PIN_XCLK,
		.pin_pclk = CAM_PIN_PCLK,
		.pin_vsync = CAM_PIN_VSYNC,
		.pin_href = CAM_PIN_HREF,
		.pin_sccb_sda = CAM_PIN_SIOD,
		.pin_sccb_scl = CAM_PIN_SIOC,
		.pin_pwdn = CAM_PIN_PWDN,
		.pin_reset = CAM_PIN_RESET,
		.xclk_freq_hz = 20000000,
		.pixel_format = PIXFORMAT_JPEG, // Output format as JPEG
		
		.frame_size = FRAMESIZE_QVGA,
		.jpeg_quality = 12,
		.fb_count = 2,
		.fb_location = CAMERA_FB_IN_PSRAM,
		.grab_mode = CAMERA_GRAB_WHEN_EMPTY
    };

    esp_err_t err = esp_camera_init(&config);
    
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Camera init failed: 0x%x", err);
        return err;
    }
    
    return ESP_OK;
}

// Connect to Wi-Fi
void wifi_init() 
{
    ESP_LOGI(TAG, "Initializing WiFi...");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

void app_main(void)
{

    while (true) 
    {
        printf("Hello from app_main!\n");
        sleep(1);
    }
}
