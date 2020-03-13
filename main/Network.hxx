#ifndef _NETWORK_HXX_
#define _NETWORK_HXX_


class RadioNetwork {
public:
  int init();
  int set(const char* ssid, const char* passwd);
  int connect();
  int disconnect();
};

extern RadioNetwork rnet;
#endif
