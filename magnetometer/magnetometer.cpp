#include "magnetometer.hpp"

namespace magnetometer
{
  void MagnetometerOutputData::from_regs(uint32_t reg28h, uint32_t reg29h)
  {
    using namespace register_traits;
    x_value = (reg28h & X_MSB_MASK) >> (X_MSB_OFFSET - AXIS_LSB_BITS);
    y_value = (reg28h & Y_MSB_MASK) >> (Y_MSB_OFFSET - AXIS_LSB_BITS);
    z_value = (reg28h & Z_MSB_MASK) >> (Z_MSB_OFFSET - AXIS_LSB_BITS);
    uint16_t temperature_temp = (reg28h & TEMPERATURE_MSB_MASK) << TEMPERATURE_LSB_BITS;

    constexpr uint32_t X_LSB_MASK = utils::get_mask(AXIS_LSB_BITS, X_LSB_OFFSET);
    constexpr uint32_t Y_LSB_MASK = utils::get_mask(AXIS_LSB_BITS, Y_LSB_OFFSET);
    constexpr uint32_t Z_LSB_MASK = utils::get_mask(AXIS_LSB_BITS, Z_LSB_OFFSET);
    constexpr uint32_t TEMPERATURE_LSB_MASK = utils::get_mask(TEMPERATURE_LSB_BITS, TEMPERATURE_LSB_OFFSET);
    x_value |= (reg29h & X_LSB_MASK) >> X_LSB_OFFSET;
    y_value |= (reg29h & Y_LSB_MASK) >> Y_LSB_OFFSET;
    z_value |= (reg29h & Z_LSB_MASK) >> Z_LSB_OFFSET;

    using SignExtender = utils::SignExtender<AXIS_LSB_BITS + AXIS_MSB_BITS>;

    x_value = get_magnetic_gauss(SignExtender::extend(x_value));
    y_value = get_magnetic_gauss(SignExtender::extend(y_value));
    z_value = get_magnetic_gauss(SignExtender::extend(z_value));

    temperature_temp |= (reg29h & TEMPERATURE_LSB_MASK) >> TEMPERATURE_LSB_OFFSET;

    temperature = convert_temperature(temperature_temp);
  }

  void MagnetometerOutputData::static_test()
  {
    static constexpr const auto MINIMUM_TEMPERATURE = convert_temperature(0);
    static constexpr const auto MAXIMUM_TEMPERATURE = convert_temperature(0xFFF);
  }

}