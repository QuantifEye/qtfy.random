#ifndef QTFY_RANDOM_PHILOX_TRAIT_HPP
#define QTFY_RANDOM_PHILOX_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <counter_word word_t, size_t words>
struct philox_constants
{
  std::array<word_t, words / 2U> bumps;
  std::array<uint64_t, words / 2U> multipliers;
};

template <counter_word word_t, size_t words>
consteval philox_constants<word_t, words> default_philox_constants()
{
  if constexpr (words == 2U && std::is_same_v<word_t, uint64_t>)
  {
    return {.bumps = {0x9E3779B97F4A7C15}, .multipliers = {0xD2B74407B1CE6E93}};
  }
  if constexpr (words == 4U && std::is_same_v<word_t, uint64_t>)
  {
    return {.bumps = {0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B}, .multipliers = {0xD2E7470EE14C6C93, 0xCA5A826395121157}};
  }
  if constexpr (words == 2U && std::is_same_v<word_t, uint32_t>)
  {
    return {.bumps = {0x9E3779B9}, .multipliers = {0xD256D193}};
  }
  if constexpr (words == 4U && std::is_same_v<word_t, uint32_t>)
  {
    return {.bumps = {0x9E3779B9, 0xBB67AE85}, .multipliers = {0xD2511F53, 0xCD9E8D57}};
  }
}

template <counter_word word_t, size_t words, unsigned rounds,
          philox_constants<word_t, words> constants = default_philox_constants<word_t, words>()>
class philox_trait
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 16U);

  static constexpr auto bump_key(auto key) noexcept
  {
    for (size_t i{}; i < words / 2U; ++i)
    {
      key[i] += constants.bumps[i];
    }

    return key;
  }

  static constexpr auto round(auto ctr, auto key) noexcept
  {
    using namespace qtfy::random::utilities;
    constexpr auto multipliers = constants.multipliers;
    if constexpr (words == 2U)
    {
      const auto product = big_mul<multipliers[0U]>(ctr[0U]);
      return counter_type{product.hi ^ key[0U] ^ ctr[1U], product.lo};
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      return counter_type{product1.hi ^ ctr[1U] ^ key[0U], product1.lo, product0.hi ^ ctr[3U] ^ key[1U], product0.lo};
    }
  }

  template <unsigned r>
  static constexpr auto round_applier(auto ctr, auto internal_key) noexcept
  {
    ctr = round(ctr, internal_key);
    if constexpr (r + 1U < rounds)
    {
      internal_key = bump_key(internal_key);
      return round_applier<r + 1>(ctr, internal_key);
    }
    else
    {
      return ctr;
    }
  }

 public:
  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words / 2>;
  using internal_key_type = key_type;

  static constexpr internal_key_type set_key(key_type key) noexcept { return key; }

  static constexpr counter_type bijection(counter_type ctr, internal_key_type internal_key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      return round_applier<0>(ctr, internal_key);
    }
    else
    {
      return ctr;
    }
  }

  template <internal_key_type key>
  static constexpr counter_type bijection(counter_type ctr) noexcept
  {
    return bijection_impl(ctr, key);
  }

  static constexpr auto make_functor(key_type key) noexcept
  {
    return [extended_key = set_key(key)](counter_type ctr) noexcept { return bijection(ctr, extended_key); };
  }

  template <key_type key>
  static constexpr auto make_functor() noexcept
  {
    return [](counter_type ctr) noexcept { return bijection<key>(ctr); };
  }
};

template <counter_word word_t, size_t words, unsigned rounds,
          philox_constants<word_t, words> constants = default_philox_constants<word_t, words>()>
constexpr auto make_philox_functor(counter<word_t, words / 2U> key) noexcept
{
  return [extended_key = set_key(key)](counter<word_t, words> ctr) noexcept {
    return philox_trait<word_t, words, rounds, constants>::bijection(ctr, extended_key);
  };
}

template <counter_word word_t, size_t words, unsigned rounds, counter<word_t, words / 2U> key,
          philox_constants<word_t, words> constants = default_philox_constants<word_t, words>()>
constexpr auto make_philox_functor() noexcept
{
  return [extended_key = set_key(key)](counter<word_t, words> ctr) noexcept {
    return philox_trait<word_t, words, rounds, constants>::template bijection<>(ctr);
  };
  return philox_trait<word_t, words, rounds, constants>::template make_functor<key>();
}

}  // namespace qtfy::random

#endif
