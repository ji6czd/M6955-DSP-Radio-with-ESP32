#include <cstdlib>
#include "freertos/portmacro.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "MainBoard.hxx"
#include "AKC6955.hxx"
#include "vars.h"
#include "Commands.hxx"


OPERATION_COMMAND OpCmd;
bool OPERATION_COMMAND::RotarySwitch(uint8_t swNum, uint8_t select)
{
  if (swNum == 0) {
  switch (select) {
  case 1:
    Radio.setFreq(594);
    break;
  case 2:
    Radio.setFreq(3700);
    break;
  case 3:
    Radio.setFreq(5800);
    break;
  case 4:
    Radio.setFreq(7200);
    break;
  case 5:
    Radio.setFreq(9000);
    break;
  case 6:
    Radio.setFreq(11000);
    break;
  case 7:
    Radio.setFreq(15000);
    break;
    case 8:
    Radio.setFreq(21500);
    break;
  case 9:
    Radio.setFreq(80000);
    break;
  default:
    break;
  }
  } else if (swNum == 1) {
    switch (select) {
    case 1:
      Radio.recallMemory(0);
      break;
    case 2:
      Radio.recallMemory(1);
      break;
    case 3:
      Radio.recallMemory(2);
      break;
    case 4:
      Radio.recallMemory(3);
      break;
    case 5:
    Radio.recallMemory(4);
    break;
    case 6:
      Radio.recallMemory(5);
      break;
    }
  }
  return true;
}

bool OPERATION_COMMAND::RotaryEncoder(bool dir)
{
  dir ? Radio.chUp() : Radio.chDown();
  return true;
}

bool OPERATION_COMMAND::NumPad(uint8_t key)
{
  switch (key) {
  case 0xa:
    Radio.addMemory();
    break;
  case 0xb:
    Radio.setFreq(inputNum);
    inputNum=0;
    break;
  default:
    inputNum = inputNum * 10 + key;
    break;
  }
  ESP_LOGI("Freq:", "%ul\n", inputNum);
  return true;
}

bool OPERATION_COMMAND::FunctionKey(uint8_t key)
{
  switch (key) {
  case 7:
    Radio.printStatus();
    break;
  }
  return true;
}
