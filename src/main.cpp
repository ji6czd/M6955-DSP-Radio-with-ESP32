#include <iostream>

extern "C" {
  int app_main()
  {
    std::cout << "Hello ESP6955 Radio" << std::endl;
    return 0;
  }
}
