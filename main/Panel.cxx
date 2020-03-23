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

class TactSwitch {
  void push() { count++; };
  void release() { count=0; cmdtoQueue(); };
private:
  int cmdtoQueue();
  uint8_t count;
  char cmdName[16];
};

int TactSwitch::cmdtoQueue()
{
  return 0;
}
