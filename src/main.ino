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
	radio.setFreq(594);
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

void loop()
{
	uint8_t cmd = NO_CMD;
	uint8_t ch=0; // 関数内で現在のチャネル番号を取得
	
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
		ch = radio.getCh();
		radio.setCh(--ch);
		break;
	case UP:
		ch = radio.getCh();
		radio.setCh(++ch);
		break;
	default:
		; // なにもしない
		break;
	}
	if(cmd) printStatus();

}
