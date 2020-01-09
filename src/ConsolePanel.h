#ifndef CONSOLE_PAANEL_H
#define CONSOLE_PAANEL_H

class ConsolePanel {
public:
	bool begin();		// (
	uint8_t readCmd();
private:
	bool readSw(uint8_t pin);
};

enum
cmd {
     POWER = 14,
     DOWN = 4,
     UP = 16,
     BAND_DOWN = 17,
     BAND_UP = 5,
     MODE = 18,
     DIAL_L = 13,
     DIAL_R = 12,
     NO_CMD = 0
};

extern ConsolePanel panel;
#endif
