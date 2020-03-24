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
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "vars.h"
#include "AKC6955.hxx"
#include "RadioConsole.hxx"
#include "Network.hxx"
#include "Panel.hxx"

const char *TAG = "radio";

int initPeripherals()
{
  gpio_config_t
    o_conf {
	    .pin_bit_mask = (1ULL << POWER_ON),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .pull_down_en = GPIO_PULLDOWN_DISABLE,
  };
  gpio_config(&o_conf);
  // Input GPIO configuration
  gpio_config_t
    i_conf {
	    .pin_bit_mask = (1ULL << POWER_SW | 1ULL << ENC_A | 1ULL << ENC_B),
	    .mode = GPIO_MODE_INPUT,
	    .pull_up_en = GPIO_PULLUP_ENABLE,
	    .pull_down_en = GPIO_PULLDOWN_DISABLE
  };
  gpio_config(&i_conf);
  // I2C configuration
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = CONFIG_I2C_MASTER_SDA;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = CONFIG_I2C_MASTER_SCL;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = CONFIG_I2C_MASTER_FREQUENCY;
  ESP_ERROR_CHECK(i2c_param_config(CONFIG_I2C_MASTER_PORT_NUM, &conf));
  return i2c_driver_install(CONFIG_I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0);
}

int initNVS()
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  return ret;
}

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t
      mount_config = {
		      .format_if_mount_failed = true,
		      .max_files = 4
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}

extern "C" {
void app_main(void)
{
  ESP_LOGI(TAG, "Starting BCL Radio!\n");
  initNVS();
  initialize_filesystem();
  rnet.init();
  rnet.connect();
  initPeripherals();
  Radio.Init();
  Radio.powerOn();
  rcon.init();
  rpan.init();
  std::cout << "Other functions" << std::endl;
}
}
