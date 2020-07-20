#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ES8388.hxx"
#include "MainBoard.hxx"

#define ES8388_ADDR 0x10
/* ES8388 register */
#define ES8388_CONTROL1         0x00
#define ES8388_CONTROL2         0x01

#define ES8388_CHIPPOWER        0x02

#define ES8388_ADCPOWER         0x03
#define ES8388_DACPOWER         0x04

#define ES8388_CHIPLOPOW1       0x05
#define ES8388_CHIPLOPOW2       0x06

#define ES8388_ANAVOLMANAG      0x07

#define ES8388_MASTERMODE       0x08
/* ADC */
#define ES8388_ADCCONTROL1      0x09
#define ES8388_ADCCONTROL2      0x0a
#define ES8388_ADCCONTROL3      0x0b
#define ES8388_ADCCONTROL4      0x0c
#define ES8388_ADCCONTROL5      0x0d
#define ES8388_ADCCONTROL6      0x0e
#define ES8388_ADCCONTROL7      0x0f
#define ES8388_ADCCONTROL8      0x10
#define ES8388_ADCCONTROL9      0x11
#define ES8388_ADCCONTROL10     0x12
#define ES8388_ADCCONTROL11     0x13
#define ES8388_ADCCONTROL12     0x14
#define ES8388_ADCCONTROL13     0x15
#define ES8388_ADCCONTROL14     0x16
/* DAC */
#define ES8388_DACCONTROL1      0x17
#define ES8388_DACCONTROL2      0x18
#define ES8388_DACCONTROL3      0x19
#define ES8388_DACCONTROL4      0x1a
#define ES8388_DACCONTROL5      0x1b
#define ES8388_DACCONTROL6      0x1c
#define ES8388_DACCONTROL7      0x1d
#define ES8388_DACCONTROL8      0x1e
#define ES8388_DACCONTROL9      0x1f
#define ES8388_DACCONTROL10     0x20
#define ES8388_DACCONTROL11     0x21
#define ES8388_DACCONTROL12     0x22
#define ES8388_DACCONTROL13     0x23
#define ES8388_DACCONTROL14     0x24
#define ES8388_DACCONTROL15     0x25
#define ES8388_DACCONTROL16     0x26
#define ES8388_DACCONTROL17     0x27
#define ES8388_DACCONTROL18     0x28
#define ES8388_DACCONTROL19     0x29
#define ES8388_DACCONTROL20     0x2a
#define ES8388_DACCONTROL21     0x2b
#define ES8388_DACCONTROL22     0x2c
#define ES8388_DACCONTROL23     0x2d
#define ES8388_DACCONTROL24     0x2e
#define ES8388_DACCONTROL25     0x2f
#define ES8388_DACCONTROL26     0x30
#define ES8388_DACCONTROL27     0x31
#define ES8388_DACCONTROL28     0x32
#define ES8388_DACCONTROL29     0x33
#define ES8388_DACCONTROL30     0x34

ES8388 es8388;
static const char tag[] = "ES8388";

int ES8388::initNormalMode()
{
  return ESP_OK;
}

int ES8388::initBypassMode()
{
  int ret = board.i2cWrite(ES8388_ADDR, ES8388_CONTROL2, 0x50); // Analog block enabled
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_MASTERMODE, 0x00); //CODEC IN I2S SLAVE MODE
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_CHIPPOWER, 0xc3); // Bypass mode?
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL17, 0x40); //  Select LIN
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL20, 0x40); //  Select RIN 
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL18, 0x38); // Not documented
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL19, 0x38); // Not documented
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL24, 0x1e); // LOUT volume 0db
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL25, 0x1e); // ROUT volume 0db
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL21, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACPOWER, 0x3c);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCPOWER, 0x3c);
  return ret;
}

int ES8388::init()
{
  ESP_LOGI(tag, "%s", "Initializing ES8388 codec...\n");
  readAll();
  int ret = board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL3, 0x04);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and  disabled digital volume control soft ramp
  //ret |= initNormalMode(); // Use DAC/ADC mode
  ret |= initBypassMode(); // Playing radio mode (analog bypass)
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL3, 0x00);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and  disabled digital volume control soft ramp
  readAll();
  if (ret != ESP_OK) ESP_LOGE("%s", "Codec initialization error!");
  return ret;
}

int ES8388::readAll()
{
  int i;
  for (i = 0; i < 50; i++) {
    uint8_t reg = 0;
    board.i2cRead(ES8388_ADDR, i, reg);
    ESP_LOGI(tag, "%x:%x", i, reg);
  }
  return i;
}
