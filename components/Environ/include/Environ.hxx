#ifndef __ENVIRON_H_
#define __ENVIRON_H_
#include <string>
#include <fstream>
#include "Types.h"
#include "cJSON.h"
using std::string;
using std::ifstream;
using std::ofstream;

struct memoryData {
  uint32_t freq;
  mode_t mode;
  string stationName;
  string streamURL;
};

class Environ {
public:
  esp_err_t init();
  ~Environ() { cJSON_free(root); };
  memoryData GetMemoryCh(uint8_t memNo);
  esp_err_t AddMemoryCh(memoryData mData);
  esp_err_t DeleteMemory(uint8_t itemNo);
  esp_err_t EditMemory(uint8_t itemNo, memoryData data);
  memoryData getStatus();
  esp_err_t setStatus(memoryData current);
  memoryData GetStatus();
  esp_err_t Save();
  esp_err_t Load();
private:
  cJSON *root, *array, *item, *current;
  cJSON * LoadFile(const char* filename);
  esp_err_t SaveFile(const char* filename, cJSON* object);
};
extern Environ env;
#endif
