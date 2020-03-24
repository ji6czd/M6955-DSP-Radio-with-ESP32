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
#include "Panel.hxx"
#include "AKC6955.hxx"

enum class
panel_cmd {
	   power_on,
	   power_off,
	   up,
	   down,
	   mode_am,
	   mod_sw,
	   mode_fm,
	   mode_inet,
	   mode_sd,
	   cmd_end
};

xQueueHandle cmd_queue;

class TactSwitch {
public:
  void init(gpio_num_t gpioPin, panel_cmd cmd) { pin = gpioPin; cmdId = cmd;};
  void checkState();
  bool isInit() { return pin ? true : false; };
  uint16_t getCount() { return count; };
private:
  int cmdtoQueue();
  uint16_t count;
  panel_cmd cmdId;
  gpio_num_t pin;
};

int TactSwitch::cmdtoQueue()
{
  xQueueSendFromISR(cmd_queue, &cmdId, NULL);
  return 0;
}

void TactSwitch::checkState()
{
  if (!gpio_get_level(pin)) {
      count++;
  } else {
    if(count > 50*10) {
    cmdtoQueue();
    }
    count=0;
  }
}

TactSwitch sw[10];

/* non-class Functions */

void timer_isr(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  // GPIO check
  for (uint8_t i=0; sw[i].isInit(); i++) {
    sw[i].checkState();
  }
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_spinlock_give(TIMER_GROUP_0);
}

Panel rpan;
void Panel::init()
{
  ESP_LOGI("Panel", "Initializing...");
  cmd_queue = xQueueCreate(4, sizeof(panel_cmd));
  timer_config_t
    tm {
	.alarm_en = TIMER_ALARM_EN,
	.counter_en = TIMER_PAUSE,
	.counter_dir = TIMER_COUNT_UP,
	.auto_reload = TIMER_AUTORELOAD_EN,
	.divider = 80
  };
  ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &tm));
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 100ULL);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x0ULL);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, 0, NULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_start(TIMER_GROUP_0, TIMER_0);
  sw[0].init(GPIO_NUM_23, panel_cmd::power_on); // Power
  sw[1].init(GPIO_NUM_12, panel_cmd::up); // encoder
  sw[2].init(GPIO_NUM_13, panel_cmd::down); // encoder
  xTaskCreate(panel_main, "PanelMain", 2048, NULL, 1, NULL);
}

void Panel::panel_main(void* args)
{
  panel_cmd cmd;
  for(;;) {
    xQueueReceive(cmd_queue, &cmd, portMAX_DELAY);
    switch (cmd) {
      case panel_cmd::power_on:
      ESP_LOGI("Panel", "Power On");
      Radio.powerToggle();
      break;
      case panel_cmd::power_off:
      ESP_LOGI("Panel", "Power Off");
      break;
      default:
	ESP_LOGI("Panel", "other command");
    }
  }
}
