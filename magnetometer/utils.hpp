#pragma once
#include <cstdint>


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
