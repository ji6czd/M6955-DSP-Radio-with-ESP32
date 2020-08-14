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
#include "driver/ledc.h"
class MainBoard
{
public:
  esp_err_t init();
  esp_err_t i2cWrite(uint8_t Device, uint8_t Register, uint8_t Data);
  esp_err_t i2cRead(uint8_t Device, uint8_t Register, uint8_t& Data);
  bool gpioGetLevel(gpio_num_t pin);
  esp_err_t gpioSetLevel(gpio_num_t pin, bool level);
private:
  esp_err_t initNVS();
  esp_err_t initFS();
  esp_err_t initI2C();
  esp_err_t initGPIO();
  esp_err_t initBeep();
  ledc_timer_config_t ledc_timer;
  ledc_channel_config_t ledc_channel;
};

extern MainBoard board;

#endif
