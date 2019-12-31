#include <arduino.h>
#include <Wire.h>
#include <m6955.h>

#define AKC6955_ADDR 0x10
#define P_ON 23

static bool SetupI2C()
{
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  Wire.begin(21,22);
  Wire.setClock(400000);
  return true;
}

uint8_t i2c_write(uint8_t device_address, uint8_t memory_address, uint8_t value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.write(value);
  uint8_t ret = Wire.endTransmission();
  delay(3);
	return ret;
}

uint8_t i2c_read(uint8_t device_address, uint8_t memory_address)
{
  uint8_t data;
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.endTransmission(false);
  Wire.requestFrom(device_address, 1, false);
  data = Wire.read();
  Wire.endTransmission(true);
  delay(2);
  return data;
}

/* class functions */
bool M6955::begin()
{
  SetupI2C();
}

bool M6955::powerOn()
{
  pinMode(P_ON, OUTPUT);
  digitalWrite(P_ON, HIGH);
	delay(3);
	if (i2c_write(AKC6955_ADDR, 0x00, 0b10000000)) {
		// Power onに失敗してる
		return false;
	}
  return true;
}
