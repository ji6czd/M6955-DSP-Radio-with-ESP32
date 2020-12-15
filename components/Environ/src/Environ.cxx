#ifndef __ENVIRON_H_
#define __ENVIRON_H_
#include <_types/_uint8_t.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cjson/cJSON.h>
using namespace std;

enum mod_t {
  mod_am,
  mod_fm,
};

enum band_t {
  bamd_MW,
  BAND_SW,
  bamd_fm
};

/*
 * BAND DEFINES
 */

enum
band_amPlan {
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

enum
band_fmPlan {
  BAND_FM1, // 87.0~108.0MHz
  BAND_FM2,     // 76.0~108.0MHz
  BAND_FM3,     // 70.0~93.0MHz
  BAND_FM4,     // 76.0~90.0MHz
  BAND_FM5,     // 64.0~88.0MHz
  BAND_TV1,     // 56.25~91.75MHz
  BAND_TV2,     // 174.75~222.25MHz
  BAND_FMUSER,  // User Setting
};

struct memoryData {
  uint16_t channelNo;
  mod_t mode;
  string stationName;
  string streamUrl;
};

class Environ {
public:
  Environ();
  ~Environ() { cJSON_free(root); };
  memoryData GetMemoryCh(uint8_t memNo);
  bool AddMemoryCh(memoryData mData);
  //uint8_t SetMemoryCh(uint8_t memNo, memoryData mData);
  //bool DeleteMemory(uint8_t itemNo);
  //bool EditMemory(uint8_t itemNo, memoryData data);
private:
  cJSON* root, *array, *item;
};
#endif

Environ::Environ()
{
  root = cJSON_CreateObject();
  array = cJSON_CreateArray();
  cJSON_AddItemToObject(root, "channels", array);
  return;
}

bool Environ::AddMemoryCh(memoryData data)
{
  cJSON * ch = cJSON_CreateObject();
  if (ch) {
    cJSON_AddStringToObject(ch, "name", data.stationName.c_str());
    cJSON_AddNumberToObject(ch, "channel_no", (double)data.channelNo);
    cJSON_AddNumberToObject(ch, "mode", (double)data.mode);
    cJSON_AddItemToArray(array, ch);
    std::cout << cJSON_PrintUnformatted(root) << std::endl;
    return true;
  }
  cout << "false" << endl;
  return false;
}

memoryData Environ::GetMemoryCh(uint8_t memNo)
{
  item = cJSON_GetArrayItem(array, memNo);
  memoryData mem;
  mem.stationName = cJSON_GetStringValue(cJSON_GetObjectItem(item, "name"));
  mem.channelNo = (uint16_t)cJSON_GetNumberValue(cJSON_GetObjectItem(item, "channel_no"));
  mem.mode = (mod_t)cJSON_GetNumberValue(cJSON_GetObjectItem(item, "mode"));
  return mem;
}

bool writetest()
{
  ofstream file("test.txt");
  if (!file) {
    cout << "open error" << endl;
    return false;
  }
  string s = "Hello world\n";
  file.write(s.c_str(), s.length());
  return true;
}

bool readtest()
{
  ifstream file("../string.cpp");
  string s;
  while (getline(file, s)) {
    cout << s << endl;
  }
  return true;
}

int main()
{
  Environ env;
  memoryData data;
  data.stationName = "Tokyo FM";
  data.channelNo = 2000;
  data.mode = mod_fm;
  env.AddMemoryCh(data);
  return 0;
}
