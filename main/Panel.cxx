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
#include "vars.h"

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
private:
  void cmdtoQueue();
  uint16_t count;
  panel_cmd cmdId;
  gpio_num_t pin;
};

void TactSwitch::cmdtoQueue()
{
  xQueueSendFromISR(cmd_queue, &cmdId, NULL);
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

class RotaryEncoder {
public:
  void init(gpio_num_t gpioPinA, gpio_num_t gpioPinB, panel_cmd cmdIdA, panel_cmd cmdIdB) {
    pinA = gpioPinA; pinB = gpioPinB;
    cmdA = cmdIdA; cmdB = cmdIdB;
  };
  void checkState();
  bool isInit() { return pinA ? true : false; };
private:
  void cmdtoQueue();
  uint16_t countA, countB;
  panel_cmd cmdA, cmdB;
  gpio_num_t pinA, pinB;
	bool statA, statB;
	int8_t dir=0;
};

void RotaryEncoder::checkState()
{
	// ピンがLowになっている時間を記録
	(!gpio_get_level(pinA)) ? countA++ : countA=0;
	(!gpio_get_level(pinB)) ? countB++ : countB=0;

	// 方向判定
	if (!dir && statA && !statB) dir=1;
	else if (dir == 1 && statB) dir=2;
	else if (dir == 2 && !statA) dir=3; // これで右回転
	else if (dir == 3 && !statB) dir=4; // これで右回転
	else if (!dir && statB && !statA) dir=-1;
	else if (dir == -1 && statA) dir=-2;
	else if(dir == -2 && !statB) dir=-3; // 左回転
	else if(dir == -3 && !statA) dir=-4; // 左回転
	else if (!statA && !statB) dir=0;
	// ピンステータスセット 3mSecでトリガー
	(countA > 3 * 10) ? statA = true : statA = false;
	(countB > 3 * 10) ? statB = true : statB = false;
	cmdtoQueue();
}

void RotaryEncoder::cmdtoQueue()
{
	if (dir == 4) xQueueSendFromISR(cmd_queue, &cmdA, NULL);
	else if (dir == -4) xQueueSendFromISR(cmd_queue, &cmdB, NULL);
}

RotaryEncoder enc[4];

/* non-class Functions */
void timer_isr(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  // GPIO check
  for (uint8_t i=0; sw[i].isInit(); i++) {
    sw[i].checkState();
  }
	enc[0].checkState();
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_spinlock_give(TIMER_GROUP_0);
}

Panel rpan;
void Panel::init()
{
  ESP_LOGI("Panel", "Initializing...");
  cmd_queue = xQueueCreate(8, sizeof(panel_cmd));
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
  enc[0].init(ENC_A, ENC_B, panel_cmd::up, panel_cmd::down);
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
		case panel_cmd::up:
			Radio.chUp();
      break;
		case panel_cmd::down:
      Radio.chDown();
      break;
		default:
	ESP_LOGI("Panel", "other command");
    }
  }
}
