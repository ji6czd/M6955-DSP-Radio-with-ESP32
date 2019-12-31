#include <arduino.h>
#include <Wire.h>
#include <m6955.h>

void setup()
{
	Serial.begin(115200);
	Serial.println("Hello M6955");
	radio.begin();
	if (!radio.powerOn()) {
		Serial.println("Power on failed");
	}
}

void loop()
{
}
