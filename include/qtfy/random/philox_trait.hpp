#ifndef QTFY_RANDOM_PHILOX_TRAIT_HPP
#define QTFY_RANDOM_PHILOX_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class word_t, size_t words, unsigned rounds, std::array<word_t, words / 2U> bumps,
          std::array<uint64_t, words / 2U> multipliers>
class philox_trait
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 16U);

  static constexpr auto round(auto ctr, auto key) noexcept
  {
    using namespace qtfy::random::utilities;

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
    for (size_t i{}; i < bumps.size(); ++i)
    {
      key[i] += bumps[i];
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
    if constexpr (rounds != 0U)
    {
      ctr = round(ctr, key);
      for (unsigned i{rounds}; --i;)
      {
        key = bump_key(key);
        ctr = round(ctr, key);
      }
    }
    return ctr;
  }
};

template <class word_t, unsigned rounds, word_t b0, uint64_t m0>
using philox2_trait = philox_trait<word_t, 2, rounds, std::array<word_t, 1>{b0}, std::array<uint64_t, 1>{m0}>;

template <class word_t, unsigned rounds, word_t b0, word_t b1, uint64_t m0, uint64_t m1>
using philox4_trait = philox_trait<word_t, 4, rounds, std::array<word_t, 2>{b0, b1}, std::array<uint64_t, 2>{m0, m1}>;

template <unsigned rounds>
using philox2x32_trait = philox2_trait<uint32_t, rounds, 0x9E3779B9, 0xD256D193>;

template <unsigned rounds>
using philox2x64_trait = philox2_trait<uint64_t, rounds, 0x9E3779B97F4A7C15, 0xD2B74407B1CE6E93>;

template <unsigned rounds>
using philox4x32_trait = philox4_trait<uint32_t, rounds, 0x9E3779B9, 0xBB67AE85, 0xD2511F53, 0xCD9E8D57>;

template <unsigned rounds>
using philox4x64_trait =
    philox4_trait<uint64_t, rounds, 0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B, 0xD2E7470EE14C6C93, 0xCA5A826395121157>;

}  // namespace qtfy::random

#endif
