#ifndef CONSOLE_PAANEL_H
#define CONSOLE_PAANEL_H

class ConsolePanel {
public:
	bool begin();		// (
	uint8_t readCmd();
private:
	uint8_t prevCmd;
	bool readSw(uint8_t pin, uint32_t th);
	bool readSw(uint8_t pin) { return readSw(pin, 10000); };
	int readDial(uint8_t pinA, uint8_t pinB);
};

enum
cmd {
     POWER = 14,
     DOWN = 4,
     UP = 16,
     BAND_DOWN = 17,
     BAND_UP = 5,
     MODE = 18,
     DIAL_L = 12,
     DIAL_R = 13,
     NO_CMD = 0
};

extern ConsolePanel panel;
#endif
