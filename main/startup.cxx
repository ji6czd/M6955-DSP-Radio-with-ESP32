/*
Internet and analogue BCL Radio startup functions
*/

#include <iostream>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "AKC6955.hxx"

int initPeripherals()
{
  gpio_config_t io_conf;
  //disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  //set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << 14);
  //disable pull-down mode
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  //disable pull-up mode
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  //configure GPIO with the given settings
  gpio_config(&io_conf);
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = CONFIG_I2C_MASTER_SDA;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = CONFIG_I2C_MASTER_SCL;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = CONFIG_I2C_MASTER_FREQUENCY;
  i2c_param_config(CONFIG_I2C_MASTER_PORT_NUM, &conf);
  std::cout << "SCL=" << CONFIG_I2C_MASTER_SCL << " SDA=" << CONFIG_I2C_MASTER_SDA << std::endl;
  return i2c_driver_install(CONFIG_I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0);
}
extern "C" {
void app_main(void)
{
  initPeripherals();
  Radio.Init();
  Radio.powerOn();
}
}
