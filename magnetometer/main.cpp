#include <iostream>
#include "magnetometer.hpp"


int main()
{
  constexpr uint32_t reg28h = 0b11100000'01001000'01001111'00011101;
  constexpr uint32_t reg29h = 0b00000000'00000110'01100110'00110111;

  magnetometer::MagnetometerOutputData data;
  data.from_regs(reg28h, reg29h);
  std::cout << "X " << data.get_x_value() << " gauss" << std::endl;
  std::cout << "Y " << data.get_y_value() << " gauss" << std::endl;
  std::cout << "Z " << data.get_z_value() << " gauss" << std::endl;
  std::cout << data.get_temperature() / 100 << ',' << data.get_temperature() % 100 << " degree C" << std::endl;
  return 0;
}
