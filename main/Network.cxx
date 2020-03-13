#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "Network.hxx"

/* Non-class C style functions */

extern "C" {
static const char *TAG = "wifi station";

static void callback_ip(void *args, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  ESP_LOGI("IP", "Event received.");
}

static void callback_WiFi(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    //esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    // esp_wifi_connect();
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
  }
}

void initWiFiSta()
{
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void setWiFiSta(const char* initSSID, const char* initPass)
{
  wifi_config_t
    wifi_config {
		 .sta = {"", "" }
  };
  strncpy((char*)wifi_config.sta.ssid, initSSID, sizeof(wifi_config.sta.ssid)-1);
  strncpy((char*)wifi_config.sta.password, initPass, sizeof(wifi_config.sta.password)-1);
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
}

void initNetIF()
{
  ESP_ERROR_CHECK(esp_netif_init());
  static esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);
}

void initEvent()
{
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, callback_WiFi, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, callback_ip, NULL));
}
}

RadioNetwork rnet;

int RadioNetwork::init()
{
  initEvent();
  initNetIF();
  initWiFiSta();
  return 0;
}

int RadioNetwork::set(const char* ssid, const char* passwd)
{
  setWiFiSta(ssid, passwd);
  return 0;
}

int RadioNetwork::connect()
{
  ESP_ERROR_CHECK(esp_wifi_start() );
  ESP_ERROR_CHECK(esp_wifi_connect());
  return 0;
}

int RadioNetwork::disconnect()
{
  esp_wifi_disconnect();
  ESP_ERROR_CHECK(esp_wifi_stop());
  return 0;
}
