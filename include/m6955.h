#ifndef _M6955_H_
#define _M6955_H_

#include <arduino.h>
#include <Wire.h>

/*
 * BAND DEFINES
 */

enum {
			AKC6955_BAND_LW = 0, // 150~285KHz, 3KHz order
			AKC6955_BAND_MW1,    // 520~1710KHz, 5KHz order
			AKC6955_BAND_MW2,    // 522~1620KHz, 9KHz order
			AKC6955_BAND_MW3,    // 520~1710KHz, 10KHz order
			AKC6955_BAND_SW1,    // 4700~10000KHz, 5KHz order
			AKC6955_BAND_SW2,    // 3200~4100KHz, 5KHz order
			AKC6955_BAND_SW3,    // 4700~5600KHz, 5KHz order
			AKC6955_BAND_SW4,    // 5700~6400KHz, 5KHz order
			AKC6955_BAND_SW5,    // 6800~7600KHz, 5KHz order
			AKC6955_BAND_SW6,    // 9200~10000KHz, 5KHz order
			AKC6955_BAND_SW7,    // 11400~12200KHz, 5KHz order
			AKC6955_BAND_SW8,    // 13500~14300KHz, 5KHz order
			AKC6955_BAND_SW9,    // 15000~15900KHz, 5KHz order
			AKC6955_BAND_SW10,   // 17400~17900KHz, 5KHz order
			AKC6955_BAND_SW11,   // 18900~19700KHz, 5KHz order
			AKC6955_BAND_SW12,   // 21400~21900KHz, 5KHz order
			AKC6955_BAND_SW13,   // 11400~17900KHz, 5KHz order
			AKC6955_BAND_AMUSER, // User Setting
			AKC6955_BAND_MW4,    // 520~1730KHz, 5KHz order
			AKC6955_BAND_FM1 = 0x20, // 87.0~108.0MHz
			AKC6955_BAND_FM2,     // 76.0~108.0MHz
			AKC6955_BAND_FM3,     // 70.0~93.0MHz
			AKC6955_BAND_FM4,     // 76.0~90.0MHz
			AKC6955_BAND_FM5,     // 64.0~88.0MHz
			AKC6955_BAND_TV1,     // 56.25~91.75MHz
			AKC6955_BAND_TV2,     // 174.75~222.25MHz
			AKC6955_BAND_FMUSER,  // User Setting
			AKC6955_BAND_END
};

class M6955 {
private:
  volatile unsigned char band;
  volatile unsigned char error;
public:
  bool begin();
  bool powerOn();
  bool powerOff();
  uint8_t getBand(void);
  uint16_t getCh(void);
  bool getMode();
  bool setBand(uint8_t band);
  bool setMode(bool mode);
  unsigned char errorStatus() { return error; }
};

extern M6955 radio;
#endif
