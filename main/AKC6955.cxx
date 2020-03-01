/*
AKC6955 controle
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

#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/

int AKC6955::Init()
{
  return 0;
}

int AKC6955::powerOn()
{
  gpio_set_level(GPIO_NUM_14, 1);
  vTaskDelay(30 / portTICK_RATE_MS);
  // Clear mute
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 0x10 << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 0x0, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 0xc8, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS));
  i2c_cmd_link_delete(cmd);
  int ret=0;
  if (ret == ESP_FAIL) {
    return ret;
  }
  vTaskDelay(30 / portTICK_RATE_MS);
  return ESP_OK;
}
AKC6955 Radio;
