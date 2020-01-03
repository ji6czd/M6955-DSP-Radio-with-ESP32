#include <arduino.h>
#include <Wire.h>
#include <m6955.h>
// Power on line GPIO
#define P_ON 23
// I2C address
#define AKC6955_ADDR 0x10
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
#define AKC6955_FM_SEEKSTEP 10
#define AKC6955_SPACE  11
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

M6955 radio;

static bool SetupI2C()
{
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  bool ret = Wire.begin(21,22, 400000);
  return ret;
}

uint8_t m6955Write(uint8_t memory_address, uint8_t value)
{
  Wire.beginTransmission(AKC6955_ADDR);
  Wire.write(memory_address);
  Wire.write(value);
  uint8_t ret = Wire.endTransmission();
	return ret;
}

uint8_t m6955Read(uint8_t memory_address)
{
  uint8_t data=0;
  Wire.beginTransmission(AKC6955_ADDR);
  Wire.write(memory_address);
  Wire.endTransmission(false);
	delay(1);
	Wire.requestFrom(AKC6955_ADDR, 1);
	delay(1);
	if (Wire.available()) {
		data = Wire.read();
	}
	Wire.endTransmission(1);
	return data;
}

/* class functions */
bool M6955::begin()
{
	if (!SetupI2C()) {
		error = 'I';
		return false;
	}
	return true;
}

bool M6955::powerOn()
{
  pinMode(P_ON, OUTPUT);
  digitalWrite(P_ON, HIGH);
	delay(0);
	akc6955Config cfg;
	 cfg.byte = m6955Read(AKC6955_CONFIG);
	 Serial.println(cfg.byte, BIN);
	cfg.bits.mute=0; // cleaer mute bit
	cfg.bits.power=1; // Set power flag
	if (m6955Write(AKC6955_CONFIG, cfg.byte)) {
		// Power onに失敗してる
		error = 'P';
		return false;
	}
  return true;
}

bool M6955::powerOff()
{
  pinMode(P_ON, OUTPUT);
  digitalWrite(P_ON, HIGH);
	delay(0);
	akc6955Config cfg;
	 cfg.byte = m6955Read(AKC6955_CONFIG);
	 Serial.println(cfg.byte, BIN);
	cfg.bits.mute=0; // cleaer mute bit
	cfg.bits.power=0; // Set power flag
	if (m6955Write(AKC6955_CONFIG, cfg.byte)) {
		// Power offに失敗してる
		error = 'P';
		return false;
	}
  return true;
}

uint8_t M6955::getBand(void)
{
	return m6955Read(AKC6955_BAND);
}

uint16_t M6955::getCh(void)
{
	uint16_t ch = m6955Read(AKC6955_CH_HI);
	ch  = ch << 8;
	ch += m6955Read(AKC6955_CH_LO);
	return ch;
}

bool M6955::getMode()
{
	akc6955Config cfg;
	cfg.byte = m6955Read(AKC6955_CONFIG);
	return cfg.bits.fm_en;
}

bool M6955::setMode(bool mode)
{
	akc6955Config cfg;
	cfg.byte = m6955Read(AKC6955_CONFIG);
	cfg.bits.fm_en = mode;
	m6955Write(AKC6955_CONFIG, cfg.byte);
	return cfg.bits.fm_en;
}

bool M6955::setBand(uint8_t band)
{
	return true;
}
