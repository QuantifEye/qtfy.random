#ifndef QTFY_RANDOM_PHILOX_TRAIT_HPP
#define QTFY_RANDOM_PHILOX_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class word_t, unsigned words, unsigned rounds, class counter_t = counter<word_t, words>,
          class key_t = counter<word_t, words / 2U>, class internal_key_t = counter<word_t, words / 2U>>
class philox_trait
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 16U);

  static constexpr auto bump_constants() noexcept
  {
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 2U)
    {
      return std::array<uint32_t, 1U>{0x9E3779B9};
    }
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 4U)
    {
      return std::array<uint32_t, 2U>{0x9E3779B9, 0xBB67AE85};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 2U)
    {
      return std::array<uint64_t, 1U>{0x9E3779B97F4A7C15};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 4U)
    {
      return std::array<uint64_t, 2U>{0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B};
    }
  }

  static constexpr auto multipliers_constants() noexcept
  {
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 2U)
    {
      return std::array<uint64_t, 1U>{0xD256D193};
    }
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 4U)
    {
      return std::array<uint64_t, 2U>{0xD2511F53, 0xCD9E8D57};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 2U)
    {
      return std::array<uint64_t, 1U>{0xD2B74407B1CE6E93};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 4U)
    {
      return std::array<uint64_t, 2U>{0xD2E7470EE14C6C93, 0xCA5A826395121157};
    }
  }

  static constexpr auto round(auto ctr, auto key) noexcept
  {
    using namespace qtfy::random::utilities;

    constexpr auto multipliers = multipliers_constants();
    if constexpr (words == 2U)
    {
      const auto product = big_mul<multipliers[0U]>(ctr[0U]);
      return counter_t{product.hi ^ key[0U] ^ ctr[1U], product.lo};
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      return counter_t{product1.hi ^ ctr[1U] ^ key[0U], product1.lo, product0.hi ^ ctr[3U] ^ key[1U], product0.lo};
    }
  }

  static constexpr auto bump_key(auto key) noexcept
  {
    constexpr auto bumps = bump_constants();
    if constexpr (words == 2U)
    {
      key[0U] += bumps[0U];
    }
    if constexpr (words == 4U)
    {
      key[0U] += bumps[0U];
      key[1U] += bumps[1U];
    }
    return key;
  }

 public:
  using counter_type = counter_t;
  using internal_key_type = internal_key_t;
  using key_type = key_t;
  using word_type = word_t;

  static constexpr internal_key_t set_key(key_t key) noexcept { return key; }

  static constexpr counter_t bijection(counter_t counter, internal_key_t key) noexcept
  {
    if constexpr (rounds == 0U)
    {
      return counter;
    }
    if constexpr (rounds == 1U)
    {
      return round(counter, key);
    }
    else
    {
      counter = round(counter, key);
      for (unsigned i{rounds - 1}; i != 0U; --i)
      {
        key = bump_key(key);
        counter = round(counter, key);
      }
      return counter;
    }
  }
};

}  // namespace qtfy::random

#endif
