#ifndef _ES8388_H_
#define _ES8388_H_

enum Input_t {
	      In1,
	      In2,
	      Dac
};

class ES8388 {
public:
  esp_err_t init();
  esp_err_t selectSrc(Input_t src);
  esp_err_t readAll();
  esp_err_t waveBeep(uint16_t freq, int16_t mSec);
private:
  int16_t WaveTable[48000];
};
extern ES8388 es8388;
#endif
