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
    current = cJSON_CreateObject();
    if (root == NULL || array == NULL || current == NULL) {
      ESP_LOGE(tag, "Faild to create new memory data.");
      return -1;
    }
    cJSON_AddItemToObject(root, "channels", array);
    cJSON_AddStringToObject(current, "name", "NHK");
    cJSON_AddNumberToObject(current, "frequency", (double)594);
    cJSON_AddNumberToObject(current, "mode", (double)2);
    cJSON_AddStringToObject(current, "streamurl", "Analogue::Radio");
    SaveFile(STATUS_DATA_FILE, current); // こっちが成功だよお
    SaveFile(MEMORY_DATA_FILE, root);
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
    SaveFile(MEMORY_DATA_FILE, root);
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

esp_err_t Environ::setStatus(memoryData data)
{
  cJSON_AddStringToObject(current, "name", data.stationName.c_str());
  cJSON_AddNumberToObject(current, "frequency", (double)data.freq);
  cJSON_AddNumberToObject(current, "mode", (double)data.mode);
  cJSON_AddStringToObject(current, "streamurl", data.streamURL.c_str());
  SaveFile(STATUS_DATA_FILE, current); // こっちが成功だよお
  return ESP_OK;
}

memoryData Environ::GetStatus()
{
  memoryData mem;
  mem.stationName = cJSON_GetObjectItem(current, "name")->valuestring;
  // mem.freq = cJSON_GetObjectItem(current, "frequency")->valueint;
  // mem.mode = (mod_t)(cJSON_GetObjectItem(current, "mode")->valueint);
  // mem.streamURL = cJSON_GetObjectItem(current, "streamurl")->valuestring;
  return mem;
}

esp_err_t Environ::SaveFile(const char* fileName, cJSON* object)
{
  ofstream fp(fileName);
  string s = cJSON_PrintUnformatted(object);
  fp.write(s.c_str(), s.length());
  ESP_LOGI(tag, "Saviing");
  return ESP_OK;
}

cJSON* Environ::LoadFile(const char* filename)
{
  ifstream file(MEMORY_DATA_FILE);
  if (!file) {
    return NULL;
  }
  string s;
  getline(file, s);
  cJSON* object = cJSON_Parse(s.c_str());
  //ESP_LOGI(tag, "%s", cJSON_PrintUnformatted(current));
  return object;
}

esp_err_t Environ::Load()
{
  root = LoadFile(MEMORY_DATA_FILE);
  if (root == NULL) {
    return -1;
  }
  array = cJSON_GetObjectItem(root, "channels");
  current = LoadFile(STATUS_DATA_FILE);
  return current ? ESP_OK : -1;
}

