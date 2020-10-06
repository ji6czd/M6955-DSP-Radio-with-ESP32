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
#include "MainBoard.hxx"
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
	   mode_test,
	   cmd_end,
	   exp_0 = 0x22,
	   exp_1 = 0x23
};

xQueueHandle cmd_queue;

class IOExpander {
public:
  void init(uint8_t Addr) { i2cAddr = Addr; cmd = panel_cmd::exp_0; triger=0; };
  void checkState();
  void checkStateISR();
  bool isInit() { return i2cAddr ? true : false; };
  uint32_t getState() { return stat; }
private:
  uint32_t stat;
  uint8_t i2cAddr;
  panel_cmd cmd;
  uint8_t triger;
};

void IOExpander::checkState()
{
  stat = 0;
  uint8_t data;
  board.i2cRead(i2cAddr, 0, data);
  stat = data;
  stat <<= 8;
  board.i2cRead(i2cAddr, 1, data);
  stat |= data;
  stat <<= 8;
  board.i2cRead(i2cAddr, 2, data);
  stat |= data;
}

void IOExpander::checkStateISR()
{
  if (triger == 100) {
    xQueueSendFromISR(cmd_queue, &cmd, NULL);
    triger = 0;
  } else {
    triger++;
  }
}

IOExpander exp[2];

class TactSwitch {
public:
  void init(uint8_t port, uint32_t pin) {
    mask = pin;;
    expander = port;
  };
  bool checkState();
  bool isInit() { return mask ? true : false; };
private:
  bool pState;
  uint32_t mask; // 接続先のピン
  uint8_t expander;
};

bool TactSwitch::checkState()
{
  bool state = ~(exp[expander].getState() & 0x00ffffff) & mask;
  if (pState != state) {
    pState = state;
    return pState ? true : false;
  }
  return false;
}

TactSwitch sw[22];

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
	(countA > 3) ? statA = true : statA = false;
	(countB > 3) ? statB = true : statB = false;
	cmdtoQueue();
}

void RotaryEncoder::cmdtoQueue()
{
	if (dir == 4) xQueueSendFromISR(cmd_queue, &cmdA, NULL);
	else if (dir == -4) xQueueSendFromISR(cmd_queue, &cmdB, NULL);
}

RotaryEncoder enc[2];

/* non-class Functions */
void IRAM_ATTR timer_isr(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  // GPIO check
  enc[0].checkState();
  exp[0].checkStateISR();
  exp[1].checkStateISR();
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  timer_spinlock_give(TIMER_GROUP_0);
}

Panel rpan;
void Panel::init()
{
  ESP_LOGI("Panel", "Initializing...");
  cmd_queue = xQueueCreate(8, sizeof(panel_cmd));
  enc[0].init(ENC_A, ENC_B, panel_cmd::up, panel_cmd::down);
  exp[0].init(0x22);
  exp[1].init(0x23);
  sw[0].init(0, 0x040000); // Nupad0
  sw[1].init(0, 0x020000); // Nupad star
  sw[2].init(0, 0x010000); // Numpad #
  sw[3].init(0, 0x080000); // Nupad 7
  sw[4].init(0, 0x100000);
  sw[5].init(0, 0x200000);
  sw[6].init(0, 0x400000);
  sw[7].init(0, 0x800000);
  sw[8].init(0, 0x000100);
  sw[9].init(0, 0x000200);
  sw[10].init(0, 0x000400);
  sw[12].init(1, 0x000001);
  sw[13].init(1, 0x000002);
  sw[14].init(1, 0x000004);
  sw[15].init(1, 0x000010);
  timer_config_t
    tm {
	.alarm_en = TIMER_ALARM_EN,
	.counter_en = TIMER_PAUSE,
	.counter_dir = TIMER_COUNT_UP,
	.auto_reload = TIMER_AUTORELOAD_EN,
	.divider = 8000
  };
  ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &tm));
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 10ULL);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x0ULL);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, 0, NULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_start(TIMER_GROUP_0, TIMER_0);
  xTaskCreate(panel_main, "PanelMain", 2048, NULL, 1, NULL);
}

void Panel::panel_main(void* args)
{
  panel_cmd cmd;
  uint32_t prevData=0;
  uint32_t data;
  for(;;) {
    xQueueReceive(cmd_queue, &cmd, portMAX_DELAY);
    switch (cmd) {
    case panel_cmd::exp_0:
      exp[0].checkState();
      exp[1].checkState();
      data = exp[1].getState();
      if (prevData != data) {
        ESP_LOGI("debug", "%08x", data);
	prevData=data;
      }
      if (sw[0].checkState())
	ESP_LOGI("Panel", "0");
      if (sw[1].checkState())
	ESP_LOGI("Panel", "sharp");
      if (sw[2].checkState())
	ESP_LOGI("Panel", "star");
      if (sw[3].checkState())
	ESP_LOGI("Panel", "7");
      if (sw[4].checkState())
	ESP_LOGI("Panel", "9");
      if (sw[5].checkState())
	ESP_LOGI("Panel", "8");
      if (sw[6].checkState())
	ESP_LOGI("Panel", "4");
      if (sw[7].checkState())
	ESP_LOGI("Panel", "6");
      if (sw[8].checkState())
	ESP_LOGI("Panel", "5");
      if (sw[9].checkState())
	ESP_LOGI("Panel", "1");
      if (sw[10].checkState())
	ESP_LOGI("Panel", "3");
     if (sw[11].checkState())
	ESP_LOGI("Panel", "2");
     if (sw[12].checkState())
	ESP_LOGI("Panel", "Select");
     if (sw[13].checkState())
	ESP_LOGI("Panel", "Down");
     if (sw[14].checkState())
	ESP_LOGI("Panel", "Up");
     if (sw[15].checkState())
	ESP_LOGI("Panel", "Voice");
      break;
    default:
      ESP_LOGI("Panel", "other command");
    }
  }
}
