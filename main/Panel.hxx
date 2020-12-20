#ifndef _PANEL_HXX_
#define _PANEL_HXX_
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

class Panel
{
public:
  void init();
private:
  static void panel_main();
  static void bandSelect(uint8_t band);
};

extern Panel rpan;
#endif
