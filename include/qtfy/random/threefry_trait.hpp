#ifndef QTFY_RANDOM_THREEFRY_TRAIT_HPP
#define QTFY_RANDOM_THREEFRY_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t parity,
          std::array<std::array<unsigned, 8>, words / 2> rotations>
class threefry_trait
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 72U);

  static consteval auto get_rotation(unsigned i, unsigned round) noexcept { return rotations.at(i).at(round % 8U); }

  template <unsigned r>
  static constexpr auto bump_counter(auto counter, auto key) noexcept
  {
    constexpr size_t internal_key_size = internal_key_type{}.size();
    constexpr word_t b = (r + 1U) / 4U;
    if constexpr (words == 2)
    {
      counter[0U] += key[(b + 0U) % internal_key_size];
      counter[1U] += key[(b + 1U) % internal_key_size] + b;
    }
    if constexpr (words == 4)
    {
      counter[0U] += key[(b + 0U) % internal_key_size];
      counter[1U] += key[(b + 1U) % internal_key_size];
      counter[2U] += key[(b + 2U) % internal_key_size];
      counter[3U] += key[(b + 3U) % internal_key_size] + b;
    }
    return counter;
  }

  template <unsigned shift>
  static constexpr auto rotate_left(auto word) noexcept
  {
    constexpr unsigned digits = std::numeric_limits<decltype(word)>::digits;
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

  template <unsigned r>
  static constexpr auto round(auto counter) noexcept
  {
    if constexpr (words == 2U)
    {
      counter[0U] += counter[1U];
      counter[1U] = rotate_left<get_rotation(0U, r)>(counter[1U]) ^ counter[0U];
    }
    if constexpr (words == 4U)
    {
      constexpr auto is_even = (r % 2U) == 0U;
      constexpr size_t index0 = 0U;
      constexpr size_t index1 = is_even ? 1U : 3U;
      constexpr size_t index2 = 2U;
      constexpr size_t index3 = is_even ? 3U : 1U;
      counter[index0] += counter[index1];
      counter[index1] = rotate_left<get_rotation(0U, r)>(counter[index1]) ^ counter[index0];
      counter[index2] += counter[index3];
      counter[index3] = rotate_left<get_rotation(1U, r)>(counter[index3]) ^ counter[index2];
    }
    return counter;
  }

  template <unsigned r>
  static constexpr auto round_applier(auto counter, auto key) noexcept
  {
    counter = round<r>(counter);
    if constexpr ((r + 1U) % 4U == 0U)
    {
      counter = bump_counter<r>(counter, key);
    }

    if constexpr (r + 1U < rounds)
    {
      return round_applier<r + 1U>(counter, key);
    }
    else
    {
      return counter;
    }
  }

 public:
  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words>;
  using internal_key_type = counter<word_t, words + 1U>;
  using word_type = word_t;

  static constexpr counter_type bijection(counter_type counter, internal_key_type key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      counter = bump_counter<0U>(counter, key);
      return round_applier<0U>(counter, key);
    }
    else
    {
      return counter;
    }
  }

  static constexpr internal_key_type set_key(key_type key) noexcept
  {
    internal_key_type result{};
    result.back() = parity;
    for (size_t i{}; i != words; ++i)
    {
      result[i] = key[i];
      result.back() ^= key[i];
    }
    return result;
  }
};

template <class word_t, unsigned rounds, word_t parity, unsigned r0, unsigned r1, unsigned r2, unsigned r3, unsigned r4,
          unsigned r5, unsigned r6, unsigned r7>
using threefry2_trait =
    threefry_trait<word_t, 2, rounds, parity,
                   std::array<std::array<unsigned, 8>, 1>{std::array<unsigned, 8>{r0, r1, r2, r3, r4, r5, r6, r7}}>;

template <class word_t, unsigned rounds, word_t parity, unsigned r00, unsigned r01, unsigned r02, unsigned r03,
          unsigned r04, unsigned r05, unsigned r06, unsigned r07, unsigned r10, unsigned r11, unsigned r12,
          unsigned r13, unsigned r14, unsigned r15, unsigned r16, unsigned r17>
using threefry4_trait = threefry_trait<word_t, 4, rounds, parity,
                                       std::array<std::array<unsigned, 8>, 2>{
                                           std::array<unsigned, 8>{r00, r01, r02, r03, r04, r05, r06, r07},
                                           std::array<unsigned, 8>{r10, r11, r12, r13, r14, r15, r16, r17}}>;

template <unsigned rounds>
using threefry4x64_trait = threefry4_trait<uint64_t, rounds, 0x1BD11BDAA9FC1A22, 14U, 52U, 23U, 5U, 25U, 46U, 58U, 32U,
                                           16U, 57U, 40U, 37U, 33U, 12U, 22U, 32U>;

template <unsigned rounds>
using threefry2x64_trait =
    threefry2_trait<uint64_t, rounds, 0x1BD11BDAA9FC1A22, 16U, 42U, 12U, 31U, 16U, 32U, 24U, 21U>;

template <unsigned rounds>
using threefry4x32_trait = threefry4_trait<uint32_t, rounds, 0x1BD11BDA, 10U, 11U, 13U, 23U, 6U, 17U, 25U, 18U, 26U,
                                           21U, 27U, 5U, 20U, 11U, 10U, 20U>;

template <unsigned rounds>
using threefry2x32_trait = threefry2_trait<uint64_t, rounds, 0x1BD11BDA, 13U, 15U, 26U, 6U, 17U, 29U, 16U, 24U>;

}  // namespace qtfy::random

#endif
