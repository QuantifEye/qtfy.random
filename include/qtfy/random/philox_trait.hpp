#ifndef QTFY_RANDOM_PHILOX_TRAIT_HPP
#define QTFY_RANDOM_PHILOX_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class word_t, size_t words, unsigned rounds,
          std::array<word_t, words / 2U> bumps,
          std::array<uint64_t, words / 2U> multipliers>
class philox_trait
{
 public:
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> ||
                std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 16U);

  using counter_type = counter<word_t, words>;
  using key_type = counter<word_t, words / 2>;
  using internal_key_type = key_type;
  using word_type = word_t;

 private:
  static constexpr internal_key_type bump_key(internal_key_type key) noexcept
  {
    for (size_t i{}; i < bumps.size(); ++i)
    {
      key[i] += bumps[i];
    }

    return key;
  }

  static constexpr counter_type round(counter_type ctr,
                                      internal_key_type key) noexcept
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
      return counter_type{product1.hi ^ ctr[1U] ^ key[0U], product1.lo,
                          product0.hi ^ ctr[3U] ^ key[1U], product0.lo};
    }
  }

  template <unsigned r, key_type key>
  static constexpr auto round_applier(auto ctr) noexcept
  {
    using namespace qtfy::random::utilities;
    if constexpr (words == 2U)
    {
      const auto product = big_mul<multipliers[0U]>(ctr[0U]);
      ctr[0U] = product.hi ^ key[0U] ^ ctr[1U];
      ctr[1U] = product.lo;
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      ctr[0U] = product1.hi ^ ctr[1U] ^ key[0U];
      ctr[1U] = product1.lo;
      ctr[2U] = product0.hi ^ ctr[3U] ^ key[1U];
      ctr[3U] = product0.lo;
    }

    if constexpr (r + 1U < rounds)
    {
      return round_applier<r + 1, bump_key(key)>(ctr);
    }
    else
    {
      return ctr;
    }
  }

  template <unsigned r>
  static constexpr auto round_applier(key_type key, auto ctr) noexcept
  {
    using namespace qtfy::random::utilities;
    if constexpr (words == 2U)
    {
      const auto product = big_mul<multipliers[0U]>(ctr[0U]);
      ctr[0U] = product.hi ^ key[0U] ^ ctr[1U];
      ctr[1U] = product.lo;
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      ctr[0U] = product1.hi ^ ctr[1U] ^ key[0U];
      ctr[1U] = product1.lo;
      ctr[2U] = product0.hi ^ ctr[3U] ^ key[1U];
      ctr[3U] = product0.lo;
    }

    if constexpr (r + 1U < rounds)
    {
      return round_applier<r + 1>(bump_key(key), ctr);
    }
    else
    {
      return ctr;
    }
  }

 public:
  static constexpr internal_key_type set_key(key_type key) noexcept
  {
    return key;
  }

  template <key_type key>
  static constexpr counter_type bijection(counter_type ctr) noexcept
  {
    if constexpr (rounds != 0U)
    {
      return round_applier<0, key>(ctr);
    }
    else
    {
      return ctr;
    }
  }

  static constexpr counter_type bijection(counter_type ctr,
                                          internal_key_type key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      return round_applier<0>(key, ctr);
    }
    else
    {
      return ctr;
    }
  }

  static constexpr auto make_bijection(key_type key) noexcept
  {
    return [extended_key = set_key(key)](counter_type ctr) noexcept {
      return bijection(ctr, extended_key);
    };
  }

  template <key_type key>
  static constexpr auto make_bijection() noexcept
  {
    return [](counter_type ctr) noexcept { return bijection<key>(ctr); };
  }
};

template <class word_t, unsigned rounds, word_t b0, uint64_t m0>
using philox2_trait = philox_trait<word_t, 2, rounds, std::array<word_t, 1>{b0},
                                   std::array<uint64_t, 1>{m0}>;

template <class word_t, unsigned rounds, word_t b0, word_t b1, uint64_t m0,
          uint64_t m1>
using philox4_trait =
    philox_trait<word_t, 4, rounds, std::array<word_t, 2>{b0, b1},
                 std::array<uint64_t, 2>{m0, m1}>;

template <unsigned rounds>
using philox2x32_trait =
    philox2_trait<uint32_t, rounds, 0x9E3779B9, 0xD256D193>;

template <unsigned rounds>
using philox2x64_trait =
    philox2_trait<uint64_t, rounds, 0x9E3779B97F4A7C15, 0xD2B74407B1CE6E93>;

template <unsigned rounds>
using philox4x32_trait = philox4_trait<uint32_t, rounds, 0x9E3779B9, 0xBB67AE85,
                                       0xD2511F53, 0xCD9E8D57>;

template <unsigned rounds>
using philox4x64_trait =
    philox4_trait<uint64_t, rounds, 0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B,
                  0xD2E7470EE14C6C93, 0xCA5A826395121157>;

template <std::unsigned_integral word_t, size_t words, unsigned rounds>
requires(std::is_same_v<uint64_t, word_t>&& words ==
         2) constexpr auto philox_factory(counter<uint64_t, 1> key) noexcept
{
  using trait_t = philox2x64_trait<rounds>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds>
requires(std::is_same_v<uint64_t, word_t>&& words ==
         4) constexpr auto philox_factory(counter<uint64_t, 2> key) noexcept
{
  using trait_t = philox4x64_trait<rounds>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds>
requires(std::is_same_v<uint32_t, word_t>&& words ==
         2) constexpr auto philox_factory(counter<uint32_t, 1> key) noexcept
{
  using trait_t = philox2x32_trait<rounds>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds>
requires(std::is_same_v<uint32_t, word_t>&& words ==
         4) constexpr auto philox_factory(counter<uint32_t, 2> key) noexcept
{
  using trait_t = philox4x32_trait<rounds>;
  return trait_t::make_bijection(key);
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          counter<uint64_t, 1> key>
requires(std::is_same_v<uint64_t, word_t>&& words ==
         2) constexpr auto philox_factory() noexcept
{
  using trait_t = philox2x64_trait<rounds>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          counter<uint64_t, 2> key>
requires(std::is_same_v<uint64_t, word_t>&& words ==
         4) constexpr auto philox_factory() noexcept
{
  using trait_t = philox4x64_trait<rounds>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          counter<uint32_t, 1> key>
requires(std::is_same_v<uint32_t, word_t>&& words ==
         2) constexpr auto philox_factory() noexcept
{
  using trait_t = philox2x32_trait<rounds>;
  return trait_t::template make_bijection<key>();
}

template <std::unsigned_integral word_t, size_t words, unsigned rounds,
          counter<uint32_t, 2> key>
requires(std::is_same_v<uint32_t, word_t>&& words ==
         4) constexpr auto philox_factory() noexcept
{
  using trait_t = philox4x32_trait<rounds>;
  return trait_t::template make_bijection<key>();
}

}  // namespace qtfy::random

#endif
