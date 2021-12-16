#ifndef QTFY_RANDOM_PHILOX_TRAIT_HPP
#define QTFY_RANDOM_PHILOX_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class word_t, unsigned words, unsigned rounds>
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
      return counter_type{product.hi ^ key[0U] ^ ctr[1U], product.lo};
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      return counter_type{product1.hi ^ ctr[1U] ^ key[0U], product1.lo, product0.hi ^ ctr[3U] ^ key[1U], product0.lo};
    }
  }

  static constexpr auto bump_key(auto key) noexcept
  {
    constexpr auto bumps = bump_constants();
    constexpr bool use_loop = true;
    if constexpr (use_loop)
    {
      // TODO: can this loop be vectorised?
      for (size_t i{}; i < words / 2U; ++i)
      {
        key[i] += bumps[i];
      }
    }
    else
    {
      if constexpr (words == 2U)
      {
        key[0U] += bumps[0U];
      }
      if constexpr (words == 4U)
      {
        key[0U] += bumps[0U];
        key[1U] += bumps[1U];
      }
    }

    return key;
  }

 public:
  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words / 2>;
  using internal_key_type = key_type;
  using word_type = word_t;

  static constexpr internal_key_type set_key(key_type key) noexcept { return key; }

  static constexpr counter_type bijection(counter_type ctr, internal_key_type key) noexcept
  {
    constexpr int version = 1;
    if constexpr (version == 1)
    {
      if constexpr (rounds != 0U)
      {
        ctr = round(ctr, key);
        for (unsigned i{rounds}; --i;)
        {
          key = bump_key(key);
          ctr = round(ctr, key);
        }
      }
    }
    if constexpr (version == 2)
    {
      for (unsigned i{}; i != rounds; ++i)
      {
        if (i != 0)
        {
          key = bump_key(key);
        }
        ctr = round(ctr, key);
      }
    }
    if constexpr (version == 3)
    {
      using namespace qtfy::random::utilities;
      constexpr auto bumps = bump_constants();
      constexpr auto multipliers = multipliers_constants();

      // TODO: will this loop unroll?
      for (unsigned i{}; i != rounds; ++i)
      {
        if (i != 0)
        {
          // TODO: can this loop be vectorised?
          for (size_t j{}; j < words / 2U; ++j)
          {
            key[j] += bumps[j];
          }
        }

        if constexpr (words == 2U)
        {
          const auto product = big_mul<multipliers[0U]>(ctr[0U]);
          ctr[0U] = product.hi ^ key[0U] ^ ctr[1U];
          ctr[1U] = product.lo;
        }
        if constexpr (words == 4U)
        {
          // TODO: can the following two lines be calculated in a single vectorised step?
          const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
          const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
          ctr[0U] = product1.hi ^ ctr[1U] ^ key[0U];
          ctr[1U] = product1.lo;
          ctr[2U] = product0.hi ^ ctr[3U] ^ key[1U];
          ctr[3U] = product0.lo;
        }
      }
    }
    return ctr;
  }
};

}  // namespace qtfy::random

#endif
