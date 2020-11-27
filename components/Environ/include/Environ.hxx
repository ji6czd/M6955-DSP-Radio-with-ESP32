#ifndef __ENVIRON_H_
#define __ENVIRON_H_
#include <string>
#include "Types.h"
using std::string;

struct memoryData {
  uint16_t ch;
  mod_t mode;
};

class Environ {
public:
  memoryData GetMemoryCh(uint8_t memNo);
};

#endif
