#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "esp_spi_flash.h"
#include "MainBoard.hxx"
#define MOUNT_PATH "/data"
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/

static char tag[] = "Mainboard";

MainBoard board;

esp_err_t MainBoard::init()
{
  esp_err_t ret;
  ret = initialize_filesystem();
  if (ret == ESP_OK) ret = i2cInit();
  return ret;
}

esp_err_t MainBoard::initialize_filesystem(void)
{
  wl_handle_t wl_handle;
  const esp_vfs_fat_mount_config_t
    mount_config = {
		    .format_if_mount_failed = 1,
		    .max_files = 16
  };
  esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
  if (err != ESP_OK) {
    ESP_LOGE(tag, "Failed to mount FATFS (%s)", esp_err_to_name(err));
    return ESP_OK;
  }
  return err;
}

int MainBoard::i2cInit()
{
  const i2c_config_t
    conf = {
	    .mode = I2C_MODE_MASTER,
	    .sda_io_num = CONFIG_I2C_MASTER_SDA,
	    .scl_io_num = CONFIG_I2C_MASTER_SCL,
	    .sda_pullup_en = GPIO_PULLUP_ENABLE,
	    .scl_pullup_en = GPIO_PULLUP_ENABLE,
	    .master = {
		       .clk_speed = 400000
		       },
  };
  ESP_ERROR_CHECK(i2c_param_config(CONFIG_I2C_MASTER_PORT_NUM, &conf));
  return i2c_driver_install(CONFIG_I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0);
}

esp_err_t MainBoard::i2cWrite(uint8_t Device, uint8_t Register, uint8_t Data)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, Device << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, Register, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, Data, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS));
  i2c_cmd_link_delete(cmd);
  return ESP_OK;
}
esp_err_t MainBoard::i2cRead(uint8_t Device, uint8_t Register, uint8_t& Data)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Device << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, Register, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, Device << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, &Data, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}
