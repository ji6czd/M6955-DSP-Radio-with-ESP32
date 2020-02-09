#include <Arduino.h>
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

M6955 radio;

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

static bool SetupI2C()
{
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  bool ret = Wire.begin(21,22, 100000);
  return ret;
}

uint8_t m6955Write(uint8_t memory_address, uint8_t value)
{
  Wire.beginTransmission(AKC6955_ADDR);
  Wire.write(memory_address);
  Wire.write(value);
  uint8_t ret = Wire.endTransmission();
  delay(1);
  return ret;
}

uint8_t m6955Read(uint8_t memory_address)
{
  uint8_t data=0;
  Wire.beginTransmission(AKC6955_ADDR);
  Wire.write(memory_address);
  Wire.endTransmission(false);
  Wire.requestFrom(AKC6955_ADDR, 1);
	if (Wire.available()) {
		data = Wire.read();
	}
	Wire.endTransmission(1);
	delay(1);
	return data;
}

bool setPhase(bool Phase)
{
	uint8_t st = m6955Read(AKC6955_VOLUME);
	if (Phase) {
		st |= 0x01;
	} else {
		st &= 0xfe;
	}
	m6955Write(AKC6955_VOLUME, st);
	delay(1);
}

bool setVolume(uint8_t vol)
{
	uint8_t st = m6955Read(AKC6955_PRE);
	if (vol > 0b11) vol=0b11;
	/* このレジスタの2,3ビットが音量を設定する */
	vol <<= 2;
	st &= 0xf3;
	st |= vol;
	m6955Write(AKC6955_VOLUME, st);
}

bool isAM3KMode()
{
	return m6955Read(AKC6955_CNR_AM) & 0x80;
}
void doTune(bool mode)
{
	akc6955Config cfg;
	cfg.bits.power=1;
	cfg.bits.fm_en = mode;
	cfg.bits.tune=0;
	m6955Write(AKC6955_CONFIG, cfg.byte);
	delay(1);
	cfg.bits.tune=1;
	m6955Write(AKC6955_CONFIG, cfg.byte);
	delay(1);
	cfg.bits.tune=0;
	m6955Write(AKC6955_CONFIG, cfg.byte);
	delay(1);
	while (!(m6955Read(AKC6955_RCH_HI) & 0x40)) {
		delay(5);
		Serial.print('.');
	}
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
	delay(1);
	akc6955Config cfg;
	 cfg.byte = m6955Read(AKC6955_CONFIG);
	cfg.bits.mute=0; // cleaer mute bit
	cfg.bits.power=1; // Set power flag
	if (m6955Write(AKC6955_CONFIG, cfg.byte)) {
		// Power onに失敗してる
		error = 'P';
		return false;
	}
	setPhase(0);
	setVolume(2);
	m6955Write(AKC6955_FM_SEEKSTEP, 0xc0);
  return true;
}

bool M6955::powerOff()
{
  pinMode(P_ON, OUTPUT);
  digitalWrite(P_ON, HIGH);
	delay(1);
	akc6955Config cfg;
	 cfg.byte = m6955Read(AKC6955_CONFIG);
	cfg.bits.mute=0; // cleaer mute bit
	cfg.bits.power=0; // Set power flag
	if (m6955Write(AKC6955_CONFIG, cfg.byte)) {
		// Power offに失敗してる
		error = 'P';
		return false;
	}
  return true;
}

bool M6955::ispowerOn()
{
		akc6955Config cfg;
	 cfg.byte = m6955Read(AKC6955_CONFIG);
	 return cfg.bits.power;
}

uint16_t M6955::getRealCh(void)
{
	uint16_t ch = m6955Read(AKC6955_RCH_HI);
	ch  = ch << 8;
	ch += m6955Read(AKC6955_RCH_LO);
	return ch & 0x1fff;
}

uint16_t M6955::setCh(uint16_t ch)
{
	akc6955Config cfg;
	cfg.byte = m6955Read(AKC6955_CONFIG);
	uint8_t c = m6955Read(AKC6955_CH_HI); // 設定値保存
	c &= 0b11100000; // 上位3ビットだけを保存
	ch &= 0x1fff; // 先頭の3ビットはチャネル番号ではない
	m6955Write(AKC6955_CH_LO, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
	ch = ch >> 8;
	ch = ch | c;
	m6955Write(AKC6955_CH_HI, (uint8_t)ch); // キャストして下位8ビットだけを書き込む
	doTune(cfg.bits.fm_en);
	channel = ch; // チューニング後のチャンネルを保存
	return getCh();
}

bool M6955::getRealMode()
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
	// 書き込みシーケンス
	doTune(mode);
	return cfg.bits.fm_en;
}

akc6955Band M6955::getRealBand(void)
{
	akc6955Band b;
	b.byte = m6955Read(AKC6955_BAND);
	return b;
}

bool M6955::setBand(akc6955Band b)
{
	akc6955Config cfg;
	cfg.byte = m6955Read(AKC6955_CONFIG);
	akc6955Band curBand;
	uint16_t ch = getCh();
	curBand.byte = m6955Read(AKC6955_BAND);
	if(cfg.bits.fm_en) {
		curBand.bits.fm = b.bits.fm;
		if(ch < bpFM[b.bits.fm].start) ch=bpFM[b.bits.fm].start;
		else if(ch > bpFM[b.bits.fm].end) ch=bpFM[b.bits.fm].end;
	} else {
		curBand.bits.am = b.bits.am;
		if(ch < bpAM[b.bits.am].start) ch=bpAM[b.bits.am].start;
		else if(ch > bpAM[b.bits.am].end) ch=bpAM[b.bits.am].end;
	}
	m6955Write(AKC6955_BAND, curBand.byte);
	doTune(cfg.bits.fm_en);
	band.byte = curBand.byte;
	setCh(ch);
	return true;
}

uint32_t M6955::getFreq()
{
	uint16_t ch = getCh();
	bool mode = getMode();
	if (mode) {
		return (ch * 25) + 30000;
	} else {
		if (isAM3KMode()) {
			return ch*3;
		}
		return ch*5;
	}
}

uint32_t M6955::getRealFreq()
{
	uint16_t ch = getRealCh();
	if (getMode()) {
		return (ch * 25) + 30000;
	} else {
		if (isAM3KMode()) {
			return ch*3;
		}
		return ch*5;
	}
}

bool M6955::setFreq(uint32_t freq)
{
	uint16_t ch;
	if (freq < 30000) {
		// LF,MF,HF	aM Mode
		setMode(false);
		if (isAM3KMode()) {
			ch = freq / 3;
		} else {
			ch = freq / 5;
		}
	} else {
		// VHF FM
		setMode(true);
		freq -= 30000;
		ch = freq / 25;
	}
	setCh(ch);
	return true;
}
