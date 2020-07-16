#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ES8388.hxx"
#include "MainBoard.hxx"

#define ES8388_ADDR 0x11
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
  /* dac */
  int ret = board.i2cWrite(ES8388_ADDR, ES8388_DACPOWER, 0xC0);  //disable DAC and disable Lout/Rout/1/2
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_CONTROL1, 0x12);  //Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
  //    ret |= board.i2cWrite(ES8388_ADDR, ES8388_CONTROL2, 0);  //LPVrefBuf=0,Pdn_ana=0
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL1, 0x18);//1a 0x18:16bit iis , 0x00:24
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL2, 0x02);  //DACFsMode,SINGLE SPEED; DACFsRatio,256
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL17, 0x90); // only left DAC to left mixer enable 0db
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL20, 0x90); // only right DAC to right mixer enable 0db
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL21, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL23, 0x00);   //vroi=0
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACPOWER, 0x3c);
  // res |= es8388_set_adc_dac_volume(ES_MODULE_DAC, 0, 0);          // 0db
  
  /* adc */
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCPOWER, 0xFF);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCCONTROL1, 0xbb); // MIC Left and Right channel PGA gain
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCCONTROL2, 0x00);  //0x00 LINSEL & RINSEL, LIN1/RIN1 as ADC Input; DSSEL,use one DS Reg11; DSR, LINPUT1-RINPUT1
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCCONTROL3, 0x02);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCCONTROL4, 0x0d); // Left/Right data, Left/Right justified mode, Bits length, I2S format
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCCONTROL5, 0x02);  //ADCFsMode,singel SPEED,RATIO=256
  //ALC for Microphone
  // res |= es8388_set_adc_dac_volume(ES_MODULE_ADC, 0, 0);      // 0db
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_ADCPOWER, 0x09); //Power on ADC, Enable LIN&RIN, Power off MICBIAS, set int1lp to low power mode
  /* enable es8388 PA */
  // es8388_pa_power(true);
  return ret;
}

int ES8388::initBypassMode()
{
  int ret = board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL16, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL17, 0x50); //  LIN to LOUT
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL20, 0x50);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL18, 0x38);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL19, 0x38);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_CHIPPOWER, 0xf3); // Bypass mode?
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL21, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
  return ret;
}

int ES8388::init()
{
  ESP_LOGI(tag, "%s", "Initializing ES8388 codec...\n");
  int ret = board.i2cWrite(ES8388_ADDR, ES8388_DACCONTROL3, 0x04);  // 0x04 mute/0x00 unmute&ramp;DAC unmute and  disabled digital volume control soft ramp
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_CONTROL2, 0x50);
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_CHIPPOWER, 0x00); //normal all and power up all
  ret |= board.i2cWrite(ES8388_ADDR, ES8388_MASTERMODE, 0x00); //CODEC IN I2S SLAVE MODE
  // ret |= initNormalMode(); // Use DAC/ADC mode
  ret |= initBypassMode(); // Playing radio mode (analog bypass)
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
