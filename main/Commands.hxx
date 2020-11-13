#ifndef _COMMANDS_HXX_
#define _COMMANDS_HXX_

class OPERATION_COMMAND {
public:
  void init() { inputNum=0; };
  bool NumPad(uint8_t key);
  bool RotarySwitch(uint8_t swNum, uint8_t select);
  bool RotaryEncoder(bool dir);
  bool FunctionKey(uint8_t key);
private:
  bool memoryMode;
  uint32_t inputNum;
};
extern OPERATION_COMMAND OpCmd;
#endif
