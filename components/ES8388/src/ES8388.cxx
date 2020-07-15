#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ES8388.hxx"
ES8388 es8388;
static const char tag[] = "ES8388";

int ES8388::init()
{
  ESP_LOGI(tag, "%s", "Initializing ES8388 codec...\n");
  return ESP_OK;
}
