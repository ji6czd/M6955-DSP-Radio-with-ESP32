#include <Arduino.h>
#include "ConsolePanel.h"
ConsolePanel panel;


bool ConsolePanel::begin()
{
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(BAND_DOWN, INPUT_PULLUP);
  pinMode(BAND_UP, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(DIAL_L, INPUT_PULLUP);
  pinMode(DIAL_R, INPUT_PULLUP);
  return true;
}

bool ConsolePanel::readSw(uint8_t pin, uint32_t th)
{
	uint32_t len = 0;
	while(len < th && !digitalRead(pin)) {
		len++;
		delayMicroseconds(1);
	}
	return (len < th ) ? false : true;
}

int ConsolePanel::readDial(uint8_t pinA, uint8_t pinB)
{
	bool pA=false, pB=false;
	int r=0; // Current state
	int dir=0;
	readSw(pinA, 50) ? pA=true : pA=false;
	readSw(pinB, 50) ? pB=true : pB=false;
	while (pA || pB) {
		readSw(pinA, 50) ? pA=true : pA=false;
		readSw(pinB, 50) ? pB=true : pB=false;
		if (r == 2 && !pA) dir=1;
		if (r == -2 && !pB) dir=-1;
		if (pA && !pB) r=1;
		if (!pA && pB) r=-1;
		if (r == 1 && pB) r=2;
		if (r == -1 && pA) r=-2;
	}
	return dir;
}

uint8_t ConsolePanel::readCmd()
{
	uint8_t cmd=NO_CMD;
  if(readSw(POWER)) {
    cmd = POWER;
  }
  else if(readSw(DOWN)) {
    cmd = DOWN;
  }
  else if(readSw(UP)) {
    cmd = UP;
  }
  else if(readSw(BAND_DOWN)) {
    cmd = BAND_DOWN;
  }
  else if(readSw(BAND_UP)) {
    cmd = BAND_UP;
  }
  else if(readSw(MODE)) {
    cmd = MODE;
  }
	int dir = readDial(DIAL_L, DIAL_R);
	if(dir < 0) cmd = DIAL_L;
	else if(dir > 0) cmd = DIAL_R;
	if (cmd != prevCmd) {
		prevCmd = cmd;
		return cmd;
	} else {
	return NO_CMD;
	}
}
