#ifndef QTFY_RANDOM_THREEFRY_TRAIT_HPP
#define QTFY_RANDOM_THREEFRY_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          std::array<word_t, 2> tweaks, word_t parity,
          std::array<std::array<unsigned, 8>, words / 2> rotations>
class threefry_trait
{
 public:
  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words>;
  using internal_key_type = counter<word_t, words + 1U>;
  using word_type = word_t;

 private:
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> ||
                std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 72U);

  static constexpr std::array<word_t, 3> extended_tweaks{tweaks[0], tweaks[1],
                                                         tweaks[0] ^ tweaks[1]};

  template <unsigned r>
  static constexpr auto bump_counter(counter_type ctr,
                                     internal_key_type key) noexcept
  {
    constexpr size_t key_size = internal_key_type{}.size();
    constexpr word_t s = (r + 1U) / 4U;
    if constexpr (words == 2)
    {
      ctr[0U] += key[(s + 0U) % key_size];
      ctr[1U] += key[(s + 1U) % key_size] + s;
    }

    if constexpr (words == 4)
    {
      ctr[0U] += key[(s + 0U) % key_size];
      ctr[1U] += key[(s + 1U) % key_size] + extended_tweaks[s % 3U];
      ctr[2U] += key[(s + 2U) % key_size] + extended_tweaks[(s + 1) % 3U];
      ctr[3U] += key[(s + 3U) % key_size] + s;
    }

    return ctr;
  }

  template <unsigned r, internal_key_type key>
  static constexpr auto bump_counter(counter_type ctr) noexcept
  {
    constexpr size_t key_size = internal_key_type{}.size();
    constexpr word_t s = (r + 1U) / 4U;
    if constexpr (words == 2)
    {
      ctr[0U] += key[(s + 0U) % key_size];
      ctr[1U] += key[(s + 1U) % key_size] + s;
    }

    if constexpr (words == 4)
    {
      ctr[0U] += key[(s + 0U) % key_size];
      ctr[1U] += key[(s + 1U) % key_size] + extended_tweaks[s % 3U];
      ctr[2U] += key[(s + 2U) % key_size] + extended_tweaks[(s + 1) % 3U];
      ctr[3U] += key[(s + 3U) % key_size] + s;
    }

    return ctr;
  }

  template <unsigned shift>
  static constexpr auto rotate_left(word_t word) noexcept
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
  static constexpr auto round(counter_type ctr) noexcept
  {
    if constexpr (words == 2U)
    {
      ctr[0U] += ctr[1U];
      ctr[1U] = rotate_left<rotations[0U][r % 8U]>(ctr[1U]) ^ ctr[0U];
    }
    if constexpr (words == 4U)
    {
      constexpr auto is_even = (r % 2U) == 0U;
      constexpr size_t idx0 = 0U;
      constexpr size_t idx1 = is_even ? 1U : 3U;
      constexpr size_t idx2 = 2U;
      constexpr size_t idx3 = is_even ? 3U : 1U;
      ctr[idx0] += ctr[idx1];
      ctr[idx1] = rotate_left<rotations[0U][r % 8U]>(ctr[idx1]) ^ ctr[idx0];
      ctr[idx2] += ctr[idx3];
      ctr[idx3] = rotate_left<rotations[1U][r % 8U]>(ctr[idx3]) ^ ctr[idx2];
    }

    return ctr;
  }

  template <unsigned r>
  static constexpr auto round_applier(counter_type counter,
                                      internal_key_type key) noexcept
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

  template <unsigned r, internal_key_type key>
  static constexpr auto round_applier(counter_type counter) noexcept
  {
    counter = round<r>(counter);
    if constexpr ((r + 1U) % 4U == 0U)
    {
      counter = bump_counter<r, key>(counter);
    }

    if constexpr (r + 1U < rounds)
    {
      return round_applier<r + 1U, key>(counter);
    }
    else
    {
      return counter;
    }
  }

 public:
  template <internal_key_type key>
  static constexpr counter_type bijection(counter_type counter) noexcept
  {
    if constexpr (rounds != 0U)
    {
      counter = bump_counter<0U, key>(counter);
      return round_applier<0U, key>(counter);
    }
    return counter;
  }

  static constexpr counter_type bijection(counter_type counter,
                                          internal_key_type key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      counter = bump_counter<0U>(counter, key);
      return round_applier<0U>(counter, key);
    }
    return counter;
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

  static constexpr auto make_bijection(key_type key) noexcept
  {
    return [extended_key = set_key(key)](counter_type ctr) noexcept {
      return bijection(ctr, extended_key);
    };
  }

  template <key_type key>
  static consteval auto make_bijection() noexcept
  {
    return
        [](counter_type ctr) noexcept { return bijection<set_key(key)>(ctr); };
  }
};

template <class word_t, unsigned rounds, word_t t0, word_t t1, word_t parity,
          unsigned r0, unsigned r1, unsigned r2, unsigned r3, unsigned r4,
          unsigned r5, unsigned r6, unsigned r7>
using threefry2_trait =
    threefry_trait<word_t, 2, rounds, std::array<word_t, 2>{t0, t1}, parity,
                   std::array<std::array<unsigned, 8>, 1>{
                       std::array<unsigned, 8>{r0, r1, r2, r3, r4, r5, r6,
                                               r7}}>;

template <class word_t, unsigned rounds, word_t t0, word_t t1, word_t parity,
          unsigned r00, unsigned r01, unsigned r02, unsigned r03, unsigned r04,
          unsigned r05, unsigned r06, unsigned r07, unsigned r10, unsigned r11,
          unsigned r12, unsigned r13, unsigned r14, unsigned r15, unsigned r16,
          unsigned r17>
using threefry4_trait = threefry_trait<
    word_t, 4, rounds, std::array<word_t, 2>{t0, t1}, parity,
    std::array<std::array<unsigned, 8>, 2>{
        std::array<unsigned, 8>{r00, r01, r02, r03, r04, r05, r06, r07},
        std::array<unsigned, 8>{r10, r11, r12, r13, r14, r15, r16, r17}}>;

template <unsigned rounds, uint64_t t0 = 0, uint64_t t1 = 0>
using threefry4x64_trait =
    threefry4_trait<uint64_t, rounds, t0, t1, 0x1BD11BDAA9FC1A22, 14U, 52U, 23U,
                    5U, 25U, 46U, 58U, 32U, 16U, 57U, 40U, 37U, 33U, 12U, 22U,
                    32U>;

template <unsigned rounds, uint64_t t0 = 0, uint64_t t1 = 0>
using threefry2x64_trait =
    threefry2_trait<uint64_t, rounds, t0, t1, 0x1BD11BDAA9FC1A22, 16U, 42U, 12U,
                    31U, 16U, 32U, 24U, 21U>;

template <unsigned rounds, uint32_t t0 = 0, uint32_t t1 = 0>
using threefry4x32_trait =
    threefry4_trait<uint32_t, rounds, t0, t1, 0x1BD11BDA, 10U, 11U, 13U, 23U,
                    6U, 17U, 25U, 18U, 26U, 21U, 27U, 5U, 20U, 11U, 10U, 20U>;

template <unsigned rounds, uint32_t t0 = 0, uint32_t t1 = 0>
using threefry2x32_trait =
    threefry2_trait<uint32_t, rounds, t0, t1, 0x1BD11BDA, 13U, 15U, 26U, 6U,
                    17U, 29U, 16U, 24U>;

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1>
requires(std::is_same_v<uint64_t, word_t>&& words == 2)
constexpr auto threefry_factory(counter<uint64_t, 2> key) noexcept
{
  using trait_t = threefry2x64_trait<rounds, t0, t1>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1>
requires(std::is_same_v<uint64_t, word_t>&& words == 4)
constexpr auto threefry_factory(counter<uint64_t, 4> key) noexcept
{
  using trait_t = threefry4x64_trait<rounds>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1>
requires(std::is_same_v<uint32_t, word_t>&& words == 2)
constexpr auto threefry_factory(counter<uint32_t, 2> key) noexcept
{
  using trait_t = threefry2x32_trait<rounds, t0, t1>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1>
requires(std::is_same_v<uint32_t, word_t>&& words == 4) constexpr auto threefry_factory(counter<uint32_t, 4> key) noexcept
{
  using trait_t = threefry4x32_trait<rounds, t0, t1>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1, counter<uint64_t, 2> key>
requires(std::is_same_v<uint64_t, word_t>&& words == 2) constexpr auto threefry_factory() noexcept
{
  using trait_t = threefry2x64_trait<rounds, t0, t1>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1, counter<uint64_t, 4> key>
requires(std::is_same_v<uint64_t, word_t>&& words == 4)
constexpr auto threefry_factory() noexcept
{
  using trait_t = threefry4x64_trait<rounds>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1, counter<uint32_t, 2> key>
requires(std::is_same_v<uint32_t, word_t>&& words == 2)
constexpr auto threefry_factory() noexcept
{
  using trait_t = threefry2x32_trait<rounds, t0, t1>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds, word_t t0, word_t t1, counter<uint32_t, 4> key>
requires(std::is_same_v<uint32_t, word_t>&& words == 4) constexpr auto threefry_factory() noexcept
{
  using trait_t = threefry4x32_trait<rounds, t0, t1>;
  return trait_t::template make_bijection<key>();
}

}  // namespace qtfy::random

#endif
