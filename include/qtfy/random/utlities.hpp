#ifndef QTFY_RANDOM_UTILITIES_HPP
#define QTFY_RANDOM_UTILITIES_HPP

#include <array>
#include <bit>
#include <cinttypes>
#include <limits>
#include <type_traits>

namespace qtfy::random {
using std::size_t;
}

namespace qtfy::random::utilities {

template <unsigned shift, std::unsigned_integral T>
constexpr T rotate_left(T word) noexcept
{
  constexpr unsigned digits = std::numeric_limits<T>::digits;
  constexpr unsigned left_shift = shift % digits;
  constexpr unsigned right_shift = digits - left_shift;
  if constexpr (left_shift != 0U)
  {
    return (word << left_shift) | (word >> right_shift);
  }
  else
  {
    return word;
  }
}

template <class T, std::endian endian = std::endian::native>
struct HiLo
{
};

template <class T>
struct HiLo<T, std::endian::big>
{
  T hi;
  T lo;
};

template <class T>
struct HiLo<T, std::endian::little>
{
  T lo;
  T hi;
};

constexpr bool has_unsigned_int128() noexcept
{
#if defined(__GNUC__) && defined(__x86_64__)
  return true;
#else
  return false;
#endif
}

template <uint64_t left, std::unsigned_integral right_t>
constexpr auto big_mul(right_t right) noexcept
{
  static_assert(std::is_same_v<right_t, uint32_t> || std::is_same_v<right_t, uint64_t>);

  using result_t = HiLo<right_t>;

  if constexpr (std::is_same_v<right_t, uint32_t>)
  {
    return std::bit_cast<result_t>(left * right);
  }
  if constexpr (std::is_same_v<right_t, uint64_t>)
  {
    if constexpr (has_unsigned_int128())
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
      constexpr auto big_left = static_cast<unsigned __int128>(left);
#pragma GCC diagnostic pop

      return std::bit_cast<result_t>(big_left * right);
    }
    else
    {
      constexpr uint64_t lower_bits = std::numeric_limits<uint32_t>::max();
      constexpr uint64_t shift = std::numeric_limits<uint32_t>::digits;
      constexpr uint64_t a_low = left & lower_bits;
      constexpr uint64_t a_high = left >> shift;
      const uint64_t b_low = right & lower_bits;
      const uint64_t b_high = right >> shift;
      const uint64_t t = a_high * b_low + (a_low * b_low >> shift);
      const uint64_t tl = a_low * b_high + (t & lower_bits);
      if constexpr (std::endian::native == std::endian::little)
      {
        return result_t{left * right, a_high * b_high + (t >> shift) + (tl >> shift)};
      }
      if constexpr (std::endian::native == std::endian::big)
      {
        return result_t{a_high * b_high + (t >> shift) + (tl >> shift), left * right};
      }
    }
  }
}

}  // namespace qtfy::random::utilities

#endif
