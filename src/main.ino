#include <arduino.h>
#include <Wire.h>
#include <m6955.h>
#include <soundOut.h>

void swSetup()
{
	pinMode(13, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);
	pinMode(27, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
}

void setup()
{
	sOut.begin();
	sOut.morseOut('s');
	Serial.begin(115200);
	Serial.println("Hello M6955");
	swSetup();
	if (!radio.begin()) {
		sOut.morseOut(radio.errorStatus());
		Serial.println("I2C initialize error.");
	}
	if (!radio.powerOn()) {
		Serial.println("Power on failed");
		sOut.morseOut(radio.errorStatus());
	}
	radio.setMode(0);
	radio.setFreq(590);
}

bool readSw(uint8_t sw)
{
	if (!digitalRead(sw)) {
		// キーが押された
		while(!digitalRead(sw))
			; // Waiting pin is high
		delay(50); // この間はキーを検出しない
	} else {
		return false;
	}
	return true;
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
	bool ret = false;
	if (readSw(13)) {
		uint16_t ch = radio.getCh();
		Serial.println(radio.setCh(++ch));
		Serial.println(radio.getFreq());
	}
	else if (readSw(12)) {
		akc6955Band band = radio.getBand();
		if (radio.getMode()) {
			if (band.bits.fm <= BAND_FMUSER) {
				band.bits.fm++;
			} else {
				band.bits.fm=0;
			}
		} else {
			if (band.bits.am <= BAND_MW4) {
				band.bits.am++;
			} else {
				band.bits.am=0;
			}
		}
		radio.setBand(band);
		Serial.println(band.bits.am);
		Serial.println(band.bits.fm);
		Serial.println("12 ON");
	}
	else if (readSw(14)) {
		bool mode = radio.getMode();
		radio.setMode(!mode);
		Serial.println(radio.getMode(), BIN);
		if (!mode) {
			radio.setFreq(76000);
		} else {
			radio.setFreq(594);
		}
		Serial.println("14 ON");
	}
	else if (readSw(27)) {
		printStatus();
		Serial.println("27 ON");
	}
}
