#ifndef AKC6955_HXX
#define AKC6955_HXX

enum mod_t {
	    mod_am,
	    mod_fm,
};

enum band_t {
	     bamd_MW,
	     bamd_fm
};

class AKC6955 {
public:
  int Init();
  int powerOn();
  int powerOff();
  int setMode(mod_t md){return 0;};
  mode_t getMode(){return 0;};
  int setBand(band_t bn){return 0;};
  uint32_t getFreq(){return 0;};
  int setFreq(uint32_t freq){return 0;};
  uint32_t chUp(){return 0;};
  uint32_t chDown(){return 0;};
private:
  uint32_t freq;
  uint16_t ch;
  int write(uint8_t memory_address, uint8_t value);
};

extern AKC6955 Radio;
#endif
