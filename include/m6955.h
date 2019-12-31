#ifndef _M6955_H_
#define _M6955_H_

#include <arduino.h>
#include <Wire.h>

class M6955 {
private:
  volatile unsigned char band;
public:
  M6955() { return; }
  bool begin();
  bool powerOn();
};

static M6955 radio;
#endif
