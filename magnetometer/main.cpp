

#include <cstdint>
#include <iostream>

namespace magnetometer
{

  namespace utils
  {
    constexpr uint32_t get_mask(uint32_t bits_count, uint32_t bits_offset)
    {
      return (0xFFFFFFFFUL >> (32 - bits_count)) << bits_offset;
    }

    static_assert(get_mask(8, 8) == 0x0000FF00UL, "implementation_error");
    static_assert(get_mask(8, 24) == 0xFF000000UL, "implementation_error");
    static_assert(get_mask(5, 1) == 0x0000003EUL, "implementation_error");


    // https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
    template <unsigned BITS_COUNT>
    class SignExtender
    {
    public:
      static int16_t extend(int16_t value)
      {
        static_assert(BITS_COUNT <= sizeof(decltype(value)) * 8, "Unsupported bits count");
        struct { int16_t value : BITS_COUNT; } s;
        s.value = value;
        return s.value;
      }
    };



  }

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
    static constexpr int32_t get_temperature(uint16_t raw_temperature_value)
    {
      const auto x = int32_t(raw_temperature_value);
      return 30200 * (x - 1708) / 4096;
    }

    static constexpr unsigned LSB_PER_GAUSS = 2; // ALS31300EEJASR-1000
    static constexpr int32_t get_magnetic_gauss(int32_t raw_gauss)
    {
      return raw_gauss / LSB_PER_GAUSS;
    }
    //static constexpr const auto MINIMUM_TEMPERATURE = get_temperature(0);
    //static constexpr const auto MAXIMUM_TEMPERATURE = get_temperature(0xFFF);



  private:
    int32_t temperature;
    int16_t x_value;
    int16_t y_value;
    int16_t z_value;
  };


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

    using SignExtender = magnetometer::utils::SignExtender<AXIS_LSB_BITS + AXIS_MSB_BITS>;

    x_value = get_magnetic_gauss(SignExtender::extend(x_value));
    y_value = get_magnetic_gauss(SignExtender::extend(y_value));
    z_value = get_magnetic_gauss(SignExtender::extend(z_value));

    temperature_temp |= (reg29h & TEMPERATURE_LSB_MASK) >> TEMPERATURE_LSB_OFFSET;

    temperature = get_temperature(temperature_temp);
  }




}


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
