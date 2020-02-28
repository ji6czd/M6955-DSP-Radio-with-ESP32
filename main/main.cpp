/*
M6955 DSP Radio
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

#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/

int initGPIO()
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
  return 0;
}

int initI2C()
{
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = CONFIG_I2C_MASTER_SDA;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = CONFIG_I2C_MASTER_SCL;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = CONFIG_I2C_MASTER_FREQUENCY;
  i2c_param_config(CONFIG_I2C_MASTER_PORT_NUM, &conf);
  std::cout << "SCL=" << CONFIG_I2C_MASTER_SCL << " SDA=" << CONFIG_I2C_MASTER_SDA << std::endl;
  return i2c_driver_install(CONFIG_I2C_MASTER_PORT_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

int powerAKC6955(bool power)
{
  gpio_set_level(GPIO_NUM_14, 1);
  vTaskDelay(30 / portTICK_RATE_MS);
  // Clear mute
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, 0x10 << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, 0x0, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, 0xc8, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  int ret = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if (ret == ESP_FAIL) {
    return ret;
  }
  vTaskDelay(30 / portTICK_RATE_MS);
  return ESP_OK;
}

extern "C" {
void app_main(void)
{
  initGPIO();
  std::cout << initI2C() << std::endl;
  std::cout << powerAKC6955(true) << std::endl;
}
}
