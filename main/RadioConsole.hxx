#ifndef CMD_HXX_
#define CMD_HXX_

class RadioConsole {
public:
  int init();
  int register_cmd();
};

extern RadioConsole rcon;
#endif
