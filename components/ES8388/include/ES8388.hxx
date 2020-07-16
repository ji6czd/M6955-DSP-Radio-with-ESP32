#ifndef _ES8388_H_
#define _ES8388_H_
class ES8388 {
public:
  int init();
  int readAll();
private:
  int initNormalMode();
  int initBypassMode();
};
extern ES8388 es8388;
#endif
