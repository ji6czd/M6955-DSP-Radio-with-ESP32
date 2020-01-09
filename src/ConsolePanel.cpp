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

bool ConsolePanel::readSw(uint8_t pin)
{
  if (!digitalRead(pin)) {
    delay(100);
    if(!digitalRead(pin)) return true;
  }
  return false;
}

uint8_t ConsolePanel::readCmd()
{
  if(readSw(POWER)) {
    return POWER;
  }
  else if(readSw(DOWN)) {
    return DOWN;
  }
  else if(readSw(UP)) {
    return UP;
  }
  if(readSw(MODE)) {
    return MODE;
  }
  return NO_CMD;
}
