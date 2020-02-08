#include <iostream>
#include <cstring>
#include <cstdlib>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

extern "C" {
  int app_main()
  {
    std::cout << "Hello ESP6955 Radio" << std::endl;
    return 0;
  }
}
