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
#include "vars.h"
#include "AKC6955.hxx"

#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/

// Register definitions.
#define AKC6955_CONFIG  0
#define AKC6955_BAND   1
#define AKC6955_CH_HI  2
#define AKC6955_CH_LO  3
#define AKC6955_UCH_ST 4
#define AKC6955_UCH_EN 5
#define AKC6955_VOLUME 6
#define AKC6955_STEREO 7
#define AKC6955_THRESH 8
#define AKC6955_ENABLE 9
#define AKC6955_FM_SEEKSTEP 11
#define AKC6955_ADCEN 12
#define AKC6955_PRE 13
// 14-19 = RESERVE

// Read only registers
#define AKC6955_RCH_HI   20
#define AKC6955_RCH_LO   21
#define AKC6955_CNR_AM   22
#define AKC6955_CNR_FM   23
#define AKC6955_PGALEVEL 24
#define AKC6955_VBAT     25
#define AKC6955_FDNUM    26
#define AKC6955_RSSI     27

struct config_bits {
	uint8_t reservee:2;
	uint8_t mute:1;
	uint8_t seekup:1;
	uint8_t seek:1;
	uint8_t tune:1;
	uint8_t fm_en:1;
	uint8_t power:1;
};

union akc6955Config {
	uint8_t byte;
	config_bits bits;
};

struct BandPlan
{
	uint16_t start;
	uint16_t end;
	uint8_t step;
};
BandPlan bpAM[] =
	{30, 93, 1,
	 102, 342, 1,
	 174, 540, 3,
	 102, 342, 2,
	 940, 2000, 1,
	 640, 820, 1,
	 940, 1120, 1,
	 1140, 1280, 1,
	 1360, 1520, 1,
	 1840, 2000, 1,
	 2280, 2440, 1,
	 2700, 2860, 1,
	 3000, 3180, 1,
	 3480, 3580, 1,
	 3780, 3940, 1,
	 4280, 4380, 1,
	 2280, 3580, 1,
	 0,0,0
	};
BandPlan bpFM[] =
	{2280, 3120, 1,
	 1840, 3120, 4,
	 0,0,0
	};

bool AKC6955::isAM3KMode()
{
  uint8_t c;
  read(AKC6955_CNR_AM, &c);
  return c & 0x80;
}

void AKC6955::doTune(bool mode)
{
  akc6955Config cfg;
  cfg.bits.power=1;
  cfg.bits.fm_en = mode;
  cfg.bits.tune=0;
  write(AKC6955_CONFIG, cfg.byte);
  cfg.bits.tune=1;
  write(AKC6955_CONFIG, cfg.byte);
  cfg.bits.tune=0;
  write(AKC6955_CONFIG, cfg.byte);
  uint8_t c=0;
  while (!(c & 0x40)) {
    read(AKC6955_RCH_HI, &c);
  }
}

int AKC6955::Init()
{
  return 0;
}

int AKC6955::write(uint8_t memory_address, uint8_t value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 0x10 << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, memory_address, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, value, ACK_CHECK_EN));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS));
  i2c_cmd_link_delete(cmd);
  int ret=0;
  if (ret == ESP_FAIL) {
    return ret;
  }
  vTaskDelay(10 / portTICK_RATE_MS);
  return ESP_OK;
}

int AKC6955::read(uint8_t memory_address, uint8_t *value)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, 0x10 << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, memory_address, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, 0x10 << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, value, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(0, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  vTaskDelay(10 / portTICK_RATE_MS);
  return ESP_OK;
}

int AKC6955::powerOn()
{
  gpio_set_level(POWER_ON, 1);
  vTaskDelay(10 / portTICK_RATE_MS);
  // Clear mute
  write(AKC6955_CONFIG, 0xc8);
  uint8_t st;
  read(AKC6955_VOLUME, &st);
  st &= 0xfe;
  write(AKC6955_VOLUME, st);
  st = read(AKC6955_PRE, &st);
  st &= 0xf3;
  st |= 0b1000;
  write(AKC6955_VOLUME, st);
  return 0;
}

int AKC6955::powerOff()
{
  gpio_set_level(POWER_ON, 0);
  vTaskDelay(10 / portTICK_RATE_MS);
  return 0;
}

void AKC6955::printStatus()
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, &cfg.byte);
  if (cfg.bits.fm_en) {
    ESP_LOGI("Radio", "FM mode.");
  } else {
    ESP_LOGI("Radio", "AM mode.");
  }    
  uint8_t c;
  read(AKC6955_CH_HI, &c);
  c &= 0x1fff; // 先頭の3ビットはチャネル番号ではない
  ch = c >> 8;
  read(AKC6955_CH_LO, &c);
  ch = ch | c;
  ESP_LOGI("Radio", "Channel: %d", ch);
}

uint16_t AKC6955::setCh(uint16_t ch)
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, &cfg.byte);
  uint8_t c;
  read(AKC6955_CH_HI, &c); // 設定値保存
  c &= 0b11100000; // 上位3ビットだけを保存
  ch &= 0x1fff; // 先頭の3ビットはチャネル番号ではない
  write(AKC6955_CH_LO, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
  ch = ch >> 8;
  ch = ch | c;
  write(AKC6955_CH_HI, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
  doTune(cfg.bits.fm_en);
  return ch;
}


bool AKC6955::setMode(bool mode)
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, &cfg.byte);
  cfg.bits.fm_en = mode;
  doTune(mode);
  return cfg.bits.fm_en;
}

int AKC6955::setFreq(uint32_t freq)
{
  uint16_t ch;
  if (freq < 30000) {
    setMode(false);
    // LF,MF,HF	aM Mode
    if (isAM3KMode()) {
      ch = freq / 3;
    } else {
      ch = freq / 5;
    }
  } else {
    // VHF FM
    setMode(true);
    freq -= 30000;
    ch = freq / 25;
  }
  setCh(ch);
  return 0;
}

AKC6955 Radio;
