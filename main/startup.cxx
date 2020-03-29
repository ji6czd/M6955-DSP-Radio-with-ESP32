/*
Internet and analogue BCL Radio startup functions
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "MainBoard.hxx"
#include "AKC6955.hxx"
#include "RadioConsole.hxx"
#include "Network.hxx"
#include "Panel.hxx"
#include "vars.h"
const char *TAG = "radio";

esp_err_t initPeripherals()
{
  gpio_config_t
    o_conf {
	    .pin_bit_mask = (1ULL << POWER_ON),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .pull_down_en = GPIO_PULLDOWN_DISABLE,
  };
  gpio_config(&o_conf);
  // Input GPIO configuration
  gpio_config_t
    i_conf {
	    .pin_bit_mask = (1ULL << POWER_SW | 1ULL << ENC_A | 1ULL << ENC_B),
	    .mode = GPIO_MODE_INPUT,
	    .pull_up_en = GPIO_PULLUP_ENABLE,
	    .pull_down_en = GPIO_PULLDOWN_DISABLE
  };
  gpio_config(&i_conf);
  return ESP_OK;
}

extern "C" {
void app_main(void)
{
  ESP_LOGI(TAG, "Starting BCL Radio!\n");
  initPeripherals();
  board.init();
  rnet.init();
  rnet.connect();
  Radio.Init();
  Radio.powerOn();
  rcon.init();
  rpan.init();
}
}
