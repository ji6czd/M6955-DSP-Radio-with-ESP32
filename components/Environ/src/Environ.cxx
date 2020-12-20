#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "esp_spi_flash.h"
#include "MainBoard.hxx"
#include "vars.h"
#include "Environ.hxx"
Environ env;

const char tag[] = "environ";
esp_err_t Environ::init()
{
  ESP_LOGI(tag, "Initializing station memory...");
  if (Load() != ESP_OK) {
    ESP_LOGI(tag, "File load error: creating new data");
    root = cJSON_CreateObject();
    array = cJSON_CreateArray();
    if (root == NULL || array == NULL) {
      ESP_LOGE(tag, "Faild to create new memory data.");
      return -1;
    }
    cJSON_AddItemToObject(root, "channels", array);
  }
  return ESP_OK;
}

esp_err_t Environ::AddMemoryCh(memoryData data)
{
  item = cJSON_CreateObject();
  if (item) {
    cJSON_AddStringToObject(item, "name", data.stationName.c_str());
    cJSON_AddNumberToObject(item, "frequency", (double)data.freq);
    cJSON_AddNumberToObject(item, "mode", (double)data.mode);
    cJSON_AddStringToObject(item, "streamurl", data.streamURL.c_str());
    cJSON_AddItemToArray(array, item);
    Save(); // こっちが成功だよお
    return ESP_OK;
   }
  return -1; // 失敗だからねえ
}

memoryData Environ::GetMemoryCh(uint8_t memNo)
{
  item = cJSON_GetArrayItem(array, memNo);
  memoryData mem;
  mem.stationName = cJSON_GetObjectItem(item, "name")->valuestring;
  mem.freq = cJSON_GetObjectItem(item, "frequency")->valueint;
  mem.mode = (mod_t)(cJSON_GetObjectItem(item, "mode")->valueint);
  mem.streamURL = cJSON_GetObjectItem(item, "streamurl")->valuestring;
  return mem;
}

esp_err_t Environ::Save()
{
  ofstream fp(MEMORY_DATA_FILE);
  string s = cJSON_PrintUnformatted(root);
  fp.write(s.c_str(), s.length());
  return ESP_OK;
}

esp_err_t Environ::Load()
{
  ifstream file(MEMORY_DATA_FILE);
  if (!file) {
    return -1;
  }
  string s;
  getline(file, s);
  root = cJSON_Parse(s.c_str());
  array = cJSON_GetObjectItem(root, "channels");
  return ESP_OK;
}

