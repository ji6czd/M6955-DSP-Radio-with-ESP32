#ifndef _NETWORK_HXX_
#define _NETWORK_HXX_


class RadioNetwork {
public:
  int connect();
  int init(const char* ssid, const char* passwd);
  int disconnect();
};

extern RadioNetwork rnet;
#endif
