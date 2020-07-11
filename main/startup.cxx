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
const char *TAG = "radio";

extern "C" {
void app_main(void)
{
  ESP_LOGI(TAG, "Starting BCL Radio!\n");
  board.init(); // メインボード初期化
  rnet.init(); // ネットワーク  TCP/IP -Wi-Fi初期化
  Radio.Init(); // アナログラジオ初期化
  rcon.init(); // シリアルコンソール初期化
  //rpan.init(); // 操作パネル初期化
  Radio.powerOn(); // アナログラジオ電源投入
  //rnet.connect(); // ネットワーク接続
}
}
