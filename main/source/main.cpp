#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "camera_config.h"
#include "esp_camera.h"
#include "esp_dma_utils.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "wifi_pass.h" // Contains WiFi password

#include "tensorflow/lite/micro/micro_interpreter.h"


static const char *TAG = "ESP32-CAM";

// Event group to signal connection
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

// Camera Initialization
esp_err_t init_camera() 
{
	esp_err_t err = ESP_OK;
	
    camera_config_t config =
    {
		.pin_pwdn = CAM_PIN_PWDN,
		.pin_reset = CAM_PIN_RESET,
		.pin_xclk = CAM_PIN_XCLK,
		.pin_sccb_sda = CAM_PIN_SIOD,
		.pin_sccb_scl = CAM_PIN_SIOC,
		.pin_d7 = CAM_PIN_D7,
		.pin_d6 = CAM_PIN_D6,
		.pin_d5 = CAM_PIN_D5,
		.pin_d4 = CAM_PIN_D4,
		.pin_d3 = CAM_PIN_D3,
		.pin_d2 = CAM_PIN_D2,
		.pin_d1 = CAM_PIN_D1,
		.pin_d0 = CAM_PIN_D0,
		.pin_vsync = CAM_PIN_VSYNC,
		.pin_href = CAM_PIN_HREF,
		.pin_pclk = CAM_PIN_PCLK,
		.xclk_freq_hz = 20000000,
		.ledc_timer = LEDC_TIMER_0,
		.ledc_channel = LEDC_CHANNEL_0,
		.pixel_format = PIXFORMAT_RGB565,
		.frame_size = FRAMESIZE_QVGA,
        .jpeg_quality = 12,
		.fb_count = 1,
		.fb_location = CAMERA_FB_IN_PSRAM,
		.grab_mode = CAMERA_GRAB_WHEN_EMPTY,
		.sccb_i2c_port = 1	
    };

    err = esp_camera_init(&config);
    
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Camera init failed: 0x%x", err);
    }
    
    return err;
}

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) 
{
    if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_START == event_id)) 
    {
        esp_wifi_connect();
    } 
    else if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_DISCONNECTED == event_id)) 
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying connection to WiFi...");
    } 
    else if ((IP_EVENT == event_base) && (IP_EVENT_STA_GOT_IP == event_id)) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta() 
{
    wifi_event_group = xEventGroupCreate();

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    
    if ((ESP_ERR_NVS_NO_FREE_PAGES == ret) || (ESP_ERR_NVS_NEW_VERSION_FOUND == ret)) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default WiFi station
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Configure WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
            WIFI_CONNECTED_BIT,
            pdFALSE,
            pdTRUE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) 
    {
        ESP_LOGI(TAG, "Connected to AP");
    } 
    else 
    {
        ESP_LOGI(TAG, "Failed to connect to AP");
    }
}

extern "C" void app_main()
{
	// Camera initalization
	ESP_ERROR_CHECK(init_camera());
    ESP_LOGI(TAG, "Camera initialized");
    
    // WiFi initialization
    wifi_init_sta();
    
    while (true) 
    {
        printf("Hello from app_main!\n");
        sleep(1);
    }
}
