#ifndef AKC6955_HXX
#define AKC6955_HXX
#include "Types.h"

struct band_bits {
  uint8_t fm:3;
	uint8_t am:5;
};

union akc6955Band {
	uint8_t byte;
	band_bits bits;
};

class AKC6955 {
public:
  int Init();
  int powerOn();
  int powerOff();
  int powerToggle();
  bool setMode(mode_t md);
  mode_t getMode() { return mode; };
  int setBand(akc6955Band bn);
  akc6955Band getBand() { return band; };
  uint32_t getFreq();
  int setFreq(uint32_t freq);
  uint16_t setCh(const uint16_t ch);
  uint16_t getCh() { return channel; };
  void chUp();
  void chDown();
  bool addMemory();
  bool recallMemory(uint8_t memNo);
  bool saveStatus();
  void printStatus();
private:
  bool powerStatus;
  uint32_t frequency;
  uint16_t channel;
  akc6955Band band;
  mode_t mode;
  int write(const uint8_t memory_address, const uint8_t value);

  int read(const uint8_t memory_address, uint8_t &value);
    bool isAM3KMode();
    void doTune(bool mode);
  uint16_t getRealCh();
};

extern AKC6955 Radio;
#endif
