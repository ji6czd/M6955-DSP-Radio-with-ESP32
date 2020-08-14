/*
AKC6955 controle
*/

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "MainBoard.hxx"
#include "AKC6955.hxx"

#define POWER_ON GPIO_NUM_23

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
  read(AKC6955_CNR_AM, c);
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
    vTaskDelay(20 / portTICK_RATE_MS);
    read(AKC6955_RCH_HI, c);
  }
}

int AKC6955::Init()
{
  return 0;
}

int AKC6955::write(uint8_t memory_address, uint8_t value)
{
  board.i2cWrite(0x10, memory_address, value);
  vTaskDelay(1 / portTICK_RATE_MS);
  return ESP_OK;
}

int AKC6955::read(uint8_t memory_address, uint8_t &value)
{
  board.i2cRead(0x10, memory_address, value);
  vTaskDelay(1 / portTICK_RATE_MS);
  return ESP_OK;
}

int AKC6955::powerOn()
{
  board.gpioSetLevel(POWER_ON, true);
  vTaskDelay(5 / portTICK_RATE_MS);
  // Clear mute
  akc6955Config cfg;
  read(AKC6955_CONFIG, cfg.byte);
  cfg.bits.power = 1;
  cfg.bits.mute = 0;
  write(AKC6955_CONFIG, cfg.byte);
  uint8_t st;
  read(AKC6955_VOLUME, st);
  st &= 0xfe;
  write(AKC6955_VOLUME, st);
  st = read(AKC6955_PRE, st);
  st &= 0xf3;
  st |= 0b1000;
  write(AKC6955_VOLUME, st);
  setFreq(594);
  return 0;
}

int AKC6955::powerOff()
{
  // Power off
  akc6955Config cfg;
  read(AKC6955_CONFIG, cfg.byte);
  cfg.bits.power = 0;
  write(AKC6955_CONFIG, cfg.byte);
  return 0;
}

int AKC6955::powerToggle()
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, cfg.byte);
  if (cfg.bits.power) {
    powerOff();
  } else {
    powerOn();
  }
  return 0;
}

void AKC6955::printStatus()
{
  ESP_LOGI("Radio", "Channel: %d", getCh());
  akc6955Band b;
  b = getBand();
  if (getMode()) {
    ESP_LOGI("Radio", "FM mode.");
    ESP_LOGI("Radio", "Band: %d", b.bits.fm);
  } else {
    ESP_LOGI("Radio", "AM mode.");
    ESP_LOGI("Radio", "Band: %d", b.bits.am);
  }
  if (isAM3KMode()) {
    ESP_LOGI("Radio", "3K step mode");
  }
  ESP_LOGI("RADIO", "Freq=%d\n", getFreq());
}

uint16_t AKC6955::setCh(uint16_t ch)
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, cfg.byte);
  uint8_t c;
  read(AKC6955_CH_HI, c); // 設定値保存
  c &= 0b11100000; // 上位3ビットだけを保存
  ch &= 0x1fff; // 先頭の3ビットはチャネル番号ではない
  write(AKC6955_CH_LO, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
  ch = ch >> 8;
  ch = ch | c;
  write(AKC6955_CH_HI, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
  doTune(cfg.bits.fm_en);
  channel = getRealCh();
  return ch;
}

uint16_t AKC6955::getRealCh()
{
  uint16_t ch=0;
  uint8_t c;
  read(AKC6955_CH_HI, c);
  c &= 0b00011111; // 先頭の3ビットはチャネル番号ではない
  ch = c;
  ch = ch << 8;
  read(AKC6955_CH_LO, c);
  ch = ch | c;
  channel = ch;
  return ch;
}

uint32_t AKC6955::getFreq()
{
  uint16_t ch = getRealCh();
  akc6955Band b;
  b = getBand();
  if (getMode()) {
    return ch*25+30000;
  } else {
    if (isAM3KMode()) {
      return ch*3;
    }
    else {
      return ch*5;
    }
  }
}

void AKC6955::chUp()
{
  uint16_t ch = getCh();
  akc6955Band b = getBand();
  
  if (isAM3KMode() && ch == 1629/3) {
    b.bits.am = 3;
    ch = 1630/5;
  }
  else if (!getMode() && ch == 30000/5) {
    setMode(true);
    ch = 1;
  } else {
    ch++;
  }
  setBand(b);
  setCh(ch);
}

void AKC6955::chDown()
{
  uint16_t ch = getCh();
  akc6955Band b = getBand();
  if (getMode() && ch == 0) {
    setMode(false);
    b.bits.am = 3;
    ch = 30000/5;
  }
  else if (!getMode() && ch == 1630/5) {
    b.bits.am = 2;
    ch = 1629/3;
  } else {
      ch--;
  }
  setBand(b);
  setCh(ch);
}

int AKC6955::setBand(akc6955Band bn)
{
  write(AKC6955_BAND, bn.byte);
  band.byte = bn.byte;
  return 0;
}

bool AKC6955::setMode(mode_t md)
{
  akc6955Config cfg;
  read(AKC6955_CONFIG, cfg.byte);
  cfg.bits.fm_en = md;
  doTune(md);
  mode = md;
  return cfg.bits.fm_en;
}

int AKC6955::setFreq(uint32_t freq)
{
  uint16_t ch;
  akc6955Band b;
    if (freq < 30000) {
    setMode(false);
    // LF,MF,HF	aM Mode
    if (freq <= 1629) {
      ch = freq / 3;
      b.bits.am = 2;
    } else {
      ch = freq / 5;
      b.bits.am = 3;
    }
  } else {
    // VHF FM
    setMode(true);
    freq -= 30000;
    ch = freq / 25;
    b.bits.fm = 1;
  }
    setBand(b);
  setCh(ch);
  printStatus();
  return 0;
}

AKC6955 Radio;
