#ifndef QTFY_RANDOM_THREEFRY_TRAIT_HPP
#define QTFY_RANDOM_THREEFRY_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <counter_word word_t, size_t words>
struct threefry_constants
{
  word_t parity;
  word_t tweaks[2];
  uint8_t rotations[words / 2U][8U];
};

template <counter_word word_t, size_t words>
consteval threefry_constants<word_t, words> default_threefry_constants(word_t tweak0 = 0U, word_t tweak1 = 0U) noexcept
{
  if constexpr (std::is_same_v<word_t, uint64_t> && words == 4)
  {
    return {.parity = 0x1BD11BDAA9FC1A22,
            .tweaks = {tweak0, tweak1},
            .rotations = {{14U, 52U, 23U, 5U, 25U, 46U, 58U, 32U}, {16U, 57U, 40U, 37U, 33U, 12U, 22U, 32U}}};
  }
  if constexpr (std::is_same_v<word_t, uint64_t> && words == 2)
  {
    return {.parity = 0x1BD11BDAA9FC1A22,
            .tweaks = {tweak0, tweak1},
            .rotations = {{16U, 42U, 12U, 31U, 16U, 32U, 24U, 21U}}};
  }
  if constexpr (std::is_same_v<word_t, uint32_t> && words == 4)
  {
    return {.parity = 0x1BD11BDA,
            .tweaks = {tweak0, tweak1},
            .rotations = {{10U, 11U, 13U, 23U, 6U, 17U, 25U, 18U}, {26U, 21U, 27U, 5U, 20U, 11U, 10U, 20U}}};
  }
  if constexpr (std::is_same_v<word_t, uint32_t> && words == 2)
  {
    return {.parity = 0x1BD11BDA, .tweaks = {tweak0, tweak1}, .rotations = {{13U, 15U, 26U, 6U, 17U, 29U, 16U, 24U}}};
  }
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          threefry_constants<word_t, words> constants = default_threefry_constants<word_t, words>()>
class threefry_trait
{
  static constexpr std::array<word_t, 3> extended_tweaks{constants.tweaks[0], constants.tweaks[1],
                                                         constants.tweaks[0] ^ constants.tweaks[1]};

  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 72U, "this is here because of the reference implementation, consider removing");

  template <unsigned r>
  [[gnu::always_inline]] static constexpr auto bump_counter(auto ctr, auto internal_key) noexcept
  {
    constexpr size_t key_size = words + 1U;
    constexpr word_t s = (r + 1U) / 4U;
    if constexpr (words == 2)
    {
      ctr[0U] += internal_key[(s + 0U) % key_size];
      ctr[1U] += internal_key[(s + 1U) % key_size] + s;
      return ctr;
    }
    else if constexpr (words == 4)
    {
      ctr[0U] += internal_key[(s + 0U) % key_size];
      ctr[1U] += internal_key[(s + 1U) % key_size] + extended_tweaks[s % 3U];
      ctr[2U] += internal_key[(s + 2U) % key_size] + extended_tweaks[(s + 1U) % 3U];
      ctr[3U] += internal_key[(s + 3U) % key_size] + s;
      return ctr;
    }
  }

  template <auto shift>
  [[gnu::always_inline]] static constexpr auto rotate_left(auto word) noexcept
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
  [[gnu::always_inline]] static constexpr auto round(auto ctr) noexcept
  {
    if constexpr (words == 2U)
    {
      ctr[0U] += ctr[1U];
      ctr[1U] = rotate_left<constants.rotations[0U][r % 8U]>(ctr[1U]) ^ ctr[0U];
    }
    if constexpr (words == 4U)
    {
      constexpr bool is_even = (r % 2U) == 0U;
      constexpr size_t idx0 = 0U;
      constexpr size_t idx1 = is_even ? 1U : 3U;
      constexpr size_t idx2 = 2U;
      constexpr size_t idx3 = is_even ? 3U : 1U;
      ctr[idx0] += ctr[idx1];
      ctr[idx1] = rotate_left<constants.rotations[0U][r % 8U]>(ctr[idx1]) ^ ctr[idx0];
      ctr[idx2] += ctr[idx3];
      ctr[idx3] = rotate_left<constants.rotations[1U][r % 8U]>(ctr[idx3]) ^ ctr[idx2];
    }

    return ctr;
  }

  template <unsigned r>
  [[gnu::always_inline]] static constexpr auto round_applier(auto ctr, auto internal_key) noexcept
  {
    ctr = round<r>(ctr);
    if constexpr ((r + 1U) % 4U == 0U)
    {
      ctr = bump_counter<r>(ctr, internal_key);
    }

    if constexpr (r + 1U < rounds)
    {
      return round_applier<r + 1U>(ctr, internal_key);
    }
    else
    {
      return ctr;
    }
  }

  [[gnu::always_inline]] static constexpr auto bijection_impl(auto ctr, auto internal_key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      ctr = bump_counter<0U>(ctr, internal_key);
      return round_applier<0U>(ctr, internal_key);
    }
    return ctr;
  }

 public:
  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words>;
  using internal_key_type = counter<word_t, words + 1U>;

  static constexpr internal_key_type set_key(key_type key) noexcept
  {
    internal_key_type result{};
    result[words] = constants.parity;
    for (size_t i{}; i != words; ++i)
    {
      result[i] = key[i];
      result[words] ^= key[i];
    }
    return result;
  }

  static constexpr counter_type bijection(counter_type ctr, internal_key_type internal_key) noexcept
  {
    return bijection_impl(ctr, internal_key);
  }

  template <internal_key_type internal_key>
  static constexpr counter_type bijection(counter_type ctr) noexcept
  {
    return bijection_impl(ctr, internal_key);
  }

  static constexpr auto make_bijection(key_type key) noexcept
  {
    return [extended_key = set_key(key)](counter_type ctr) noexcept { return bijection(ctr, extended_key); };
  }

  template <key_type key>
  static constexpr auto make_bijection() noexcept
  {
    return [](counter_type ctr) noexcept { return bijection<set_key(key)>(ctr); };
  }
};

template <counter_word word_t, size_t words, unsigned rounds,
          threefry_constants<word_t, words> constants = default_threefry_constants<word_t, words>()>
constexpr std::regular_invocable<counter<word_t, words>> auto make_threefry_functor(counter<word_t, words> key) noexcept
{
  return [](counter<word_t, words> ctr) noexcept { return bijection<set_key(key)>(ctr); };
}

template <counter_word word_t, size_t words, unsigned rounds, counter<word_t, words> key,
          threefry_constants<word_t, words> constants = default_threefry_constants<word_t, words>()>
constexpr std::regular_invocable<counter<word_t, words>> auto make_threefry_functor() noexcept
{
  return threefry_trait<word_t, words, rounds, constants>::template make_bijection<key>();
}

}  // namespace qtfy::random

#endif
