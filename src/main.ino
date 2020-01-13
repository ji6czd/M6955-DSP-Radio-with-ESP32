#include <arduino.h>
#include <Wire.h>
#include <m6955.h>
#include <soundOut.h>
#include "ConsolePanel.h"

void setup()
{
	sOut.begin();
	sOut.morseOut('s');
	Serial.begin(115200);
	Serial.println("Hello M6955");
	panel.begin();
	if (!radio.begin()) {
		sOut.morseOut(radio.errorStatus());
		Serial.println("I2C initialize error.");
	}
	if (!radio.powerOn()) {
		Serial.println("Power on failed");
		sOut.morseOut(radio.errorStatus());
		return;
	}
	radio.setMode(0);
	akc6955Band band;
	band.bits.am = BAND_SW3;
	band.bits.fm = BAND_FM2;
	radio.setBand(band);
	radio.setFreq(7275);
}

void printStatus()
{
	bool mode = radio.getMode();
	akc6955Band curBand;
	curBand = radio.getBand();
	uint16_t curCh = radio.getCh();
	Serial.println(mode);
	Serial.println(curBand.bits.am);
	Serial.println(curBand.bits.fm);
	Serial.println(curCh);
	Serial.println(radio.getFreq());
	return;
}

void bandToggle(bool direction)
{
	akc6955Band band = radio.getBand();
	if (radio.getMode()) {
		direction ? band.bits.fm++ : band.bits.fm--;
		if (band.bits.fm > BAND_TV2) return;
	} else {
		direction ? band.bits.am++ : band.bits.am--;
		if (band.bits.am > BAND_SW13) return;
	}
		Serial.print(band.bits.am);
		Serial.print(':');
		Serial.println(band.bits.fm);
	radio.setBand(band);
}

void loop()
{
	uint8_t cmd = NO_CMD;
	uint16_t ch=0; // 関数内で現在のチャネル番号を取得
	
	cmd = panel.readCmd();
	switch (cmd) {
	case POWER:
		if (radio.ispowerOn()) {
			radio.powerOff();
			Serial.println("Power off");
		} else {
			radio.powerOn();
			Serial.println("Power On");
		}
		break;
	case DOWN:
	case DIAL_L:
		ch = radio.getCh();
		radio.setCh(--ch);
		Serial.println(radio.getFreq());
		break;
	case UP:
	case DIAL_R:
		ch = radio.getCh();
		radio.setCh(++ch);
		Serial.println(radio.getFreq());
		break;
	case MODE:
		radio.setMode(!radio.getMode());
		break;
	case BAND_UP:
		bandToggle(true);
		break;
	case BAND_DOWN:
		bandToggle(false);
		break;
	default:
		; // なにもしない
		break;
	}
	if(cmd) {
	}
}
