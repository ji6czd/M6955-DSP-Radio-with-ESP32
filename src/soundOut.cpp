#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"
#include "soundOut.h"

#include <thread>
#include <cstdint>
#include "MorseTable.h"

using namespace std;
int soundOut::freq, soundOut::mSec, soundOut::vol;
unsigned int soundOut::tone=800;
unsigned int soundOut::cpm=70;

soundOut sOut;

void soundOut::beep(int f, int m)
{
  freq=f;
  mSec=m;
  thread th(beepFunc);
  th.detach();
}

void soundOut::waitBeep(int f, int m)
{
  freq=f;
  mSec=m;
  thread th(beepFunc);
  th.join();
}

void soundOut::beepFunc()
{
  ledcWriteTone(0, freq);
  ledcWrite(0, vol);
  delay(mSec);
  ledcWriteTone(0, 0);
}

void soundOut::morseOut(const char c)
{
  if (isspace(c)) {
    morseSpace();
  }
  else if (isalnum(c)) {
    unsigned char s = MorseTable[toupper(c)-0x30];
		while (s != 1) {
			if (s & 0b1) {
				morseLong();
			} else {
				morseShort();
			}
			s >>= 1;
		}
	}
	morseSpace();
}

void soundOut::morseShort() {
	waitBeep(tone, 1000/(cpm/2));
	delay(1000/(cpm/2));
}

void soundOut::morseLong() {
	waitBeep(tone, 1000/(cpm/2)*3);
	delay(1000/(cpm/2));
}

void soundOut::morseSpace()
{
	delay(1000/(cpm/2)*3);
}

void soundOut::begin()
{
	ledcSetup(0, 1000, 13);
	ledcAttachPin(25, 0);
	vol=1;
}
