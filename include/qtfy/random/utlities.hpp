#ifndef QTFY_RANDOM_UTILITIES_HPP
#define QTFY_RANDOM_UTILITIES_HPP

#include <array>
#include <bit>
#include <cinttypes>
#include <cmath>
#include <limits>
#include <type_traits>

namespace qtfy::random {
using std::size_t;
}

namespace qtfy::random::utilities {

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
  static_assert(std::is_same_v<right_t, uint32_t> ||
                std::is_same_v<right_t, uint64_t>);

  using result_t = HiLo<right_t>;

  if constexpr (std::is_same_v<right_t, uint32_t>)
  {
    return std::bit_cast<result_t>(left * right);
  }
  if constexpr (std::is_same_v<right_t, uint64_t>)
  {
    constexpr bool use_bit_cast = false;
    if constexpr (has_unsigned_int128() && use_bit_cast)
    {
      constexpr auto big_left = static_cast<unsigned __int128>(left);
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
      const uint64_t low = left * right;
      const uint64_t high = a_high * b_high + (t >> shift) + (tl >> shift);
      if constexpr (std::endian::native == std::endian::little)
      {
        return result_t{low, high};
      }
      //TODO: big endian logic has not been tested.
      if constexpr (std::endian::native == std::endian::big)
      {
        return result_t{high, low};
      }
    }
  }
}

}  // namespace qtfy::random::utilities

#endif
