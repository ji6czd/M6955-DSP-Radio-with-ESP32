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
#include "Network.hxx"
#include "ES8388.hxx"
static const char *TAG = "radio";

extern "C" {
void app_main(void)
{
  ESP_LOGI(TAG, "Starting BCL Radio!\n");
  board.init(); // メインボード初期化
  es8388.init();
}
}
