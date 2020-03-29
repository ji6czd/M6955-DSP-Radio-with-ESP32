#ifndef _MAIN_BOARD_HXX_
#define _MAIN_BOARD_HXX_

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

class MainBoard
{
public:
  esp_err_t init();
  esp_err_t i2cWrite(uint8_t Device, uint8_t Register, uint8_t Data);
  esp_err_t i2cRead(uint8_t Device, uint8_t Register, uint8_t& Data);
private:
  esp_err_t initNVS();
  esp_err_t initialize_filesystem();
  esp_err_t i2cInit();
};

extern MainBoard board;

#endif
