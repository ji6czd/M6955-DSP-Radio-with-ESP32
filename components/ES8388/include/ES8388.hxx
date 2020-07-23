#ifndef _ES8388_H_
#define _ES8388_H_

enum Input_t {
	      In1,
	      In2,
	      Dac
};

class ES8388 {
public:
  int init();
  int selectSrc(Input_t src);
  int readAll();
private:
};
extern ES8388 es8388;
#endif
