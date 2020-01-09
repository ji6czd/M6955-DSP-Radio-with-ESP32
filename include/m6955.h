#ifndef _M6955_H_
#define _M6955_H_

#include <arduino.h>
#include <Wire.h>

/*
 * BAND DEFINES
 */

enum band_am {
							BAND_LW = 0, // 150~285KHz, 3KHz order
							BAND_MW1,    // 520~1710KHz, 5KHz order
							BAND_MW2,    // 522~1620KHz, 9KHz order
							BAND_MW3,    // 520~1710KHz, 10KHz order
							BAND_SW1,    // 4700~10000KHz, 5KHz order
							BAND_SW2,    // 3200~4100KHz, 5KHz order
							BAND_SW3,    // 4700~5600KHz, 5KHz order
							BAND_SW4,    // 5700~6400KHz, 5KHz order
							BAND_SW5,    // 6800~7600KHz, 5KHz order
							BAND_SW6,    // 9200~10000KHz, 5KHz order
							BAND_SW7,    // 11400~12200KHz, 5KHz order
							BAND_SW8,    // 13500~14300KHz, 5KHz order
							BAND_SW9,    // 15000~15900KHz, 5KHz order
							BAND_SW10,   // 17400~17900KHz, 5KHz order
							BAND_SW11,   // 18900~19700KHz, 5KHz order
							BAND_SW12,   // 21400~21900KHz, 5KHz order
							BAND_SW13,   // 11400~17900KHz, 5KHz order
							BAND_AMUSER, // User Setting
							BAND_MW4,    // 520~1730KHz, 5KHz order
};

enum band_fm {
							BAND_FM1, // 87.0~108.0MHz
							BAND_FM2,     // 76.0~108.0MHz
							BAND_FM3,     // 70.0~93.0MHz
							BAND_FM4,     // 76.0~90.0MHz
							BAND_FM5,     // 64.0~88.0MHz
							BAND_TV1,     // 56.25~91.75MHz
							BAND_TV2,     // 174.75~222.25MHz
							BAND_FMUSER,  // User Setting
};

struct band_bits {
	uint8_t fm:3;
	uint8_t am:5;
};

union akc6955Band {
	uint8_t byte;
	band_bits bits;
};

class M6955 {
private:
  volatile unsigned char band;
  volatile unsigned char error;
public:
  bool begin();
  bool powerOn();
  bool powerOff();
	bool ispowerOn();
  akc6955Band getBand(void);
  uint16_t getCh(void);
  bool getMode();
  bool setBand(akc6955Band b);
  bool setMode(bool mode);
	uint16_t setCh(uint16_t ch);
	uint32_t getFreq();
	bool setFreq(uint32_t freq);

  unsigned char errorStatus() { return error; }
};

extern M6955 radio;
#endif
