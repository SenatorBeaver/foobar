#pragma once

#include "utils.hpp"

namespace magnetometer
{

  namespace register_traits
  {
    constexpr unsigned AXIS_MSB_BITS = 8;
    constexpr unsigned AXIS_LSB_BITS = 4;
    constexpr unsigned TEMPERATURE_MSB_BITS = 6;
    constexpr unsigned TEMPERATURE_LSB_BITS = 6;
    constexpr uint32_t X_MSB_OFFSET = 24;
    constexpr uint32_t Y_MSB_OFFSET = 16;
    constexpr uint32_t Z_MSB_OFFSET = 8;
    constexpr uint32_t X_LSB_OFFSET = 16;
    constexpr uint32_t Y_LSB_OFFSET = 12;
    constexpr uint32_t Z_LSB_OFFSET = 8;
    constexpr uint32_t TEMPERATURE_MSB_OFFSET = 0;
    constexpr uint32_t TEMPERATURE_LSB_OFFSET = 0;

    constexpr uint32_t X_MSB_MASK = utils::get_mask(AXIS_MSB_BITS, X_MSB_OFFSET);
    constexpr uint32_t Y_MSB_MASK = utils::get_mask(AXIS_MSB_BITS, Y_MSB_OFFSET);
    constexpr uint32_t Z_MSB_MASK = utils::get_mask(AXIS_MSB_BITS, Z_MSB_OFFSET);
    constexpr uint32_t TEMPERATURE_MSB_MASK = utils::get_mask(TEMPERATURE_MSB_BITS, TEMPERATURE_MSB_OFFSET);
  }


  class MagnetometerOutputData
  {
  public:
    [[nodiscard]] int32_t get_temperature() const
    {
      return temperature;
    }

    [[nodiscard]] int16_t get_x_value() const
    {
      return x_value;
    }

    [[nodiscard]] int16_t get_y_value() const
    {
      return y_value;
    }

    [[nodiscard]] int16_t get_z_value() const
    {
      return z_value;
    }

    void from_regs(uint32_t reg28h, uint32_t reg29h);
  private:
    // in 0,01 celsius units
    static constexpr int32_t convert_temperature(uint16_t raw_temperature_value)
    {
      const auto x = int32_t(raw_temperature_value);
      return 30200 * (x - 1708) / 4096;
    }

    static constexpr unsigned LSB_PER_GAUSS = 2; // ALS31300EEJASR-1000
    static constexpr int32_t get_magnetic_gauss(int32_t raw_gauss)
    {
      return raw_gauss / LSB_PER_GAUSS;
    }

    void static_test();

  private:
    int32_t temperature;
    int16_t x_value;
    int16_t y_value;
    int16_t z_value;
  };


}
