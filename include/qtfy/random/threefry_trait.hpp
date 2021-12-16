#ifndef QTFY_RANDOM_THREEFRY_TRAIT_HPP
#define QTFY_RANDOM_THREEFRY_TRAIT_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class word_t, unsigned words, unsigned rounds>
class threefry_trait
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);
  static_assert(rounds <= 72U);

  static constexpr auto parity() noexcept
  {
    if constexpr (std::is_same_v<word_t, uint32_t>)
    {
      return static_cast<uint32_t>(0x1BD11BDA);
    }
    if constexpr (std::is_same_v<word_t, uint64_t>)
    {
      return static_cast<uint64_t>(0x1BD11BDAA9FC1A22);
    }
  }

  static constexpr auto rotations_constants() noexcept
  {
    using std::array;
    using arr_t = array<unsigned, 8U>;
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 2U)
    {
      return array<arr_t, 1>{arr_t{13U, 15U, 26U, 6U, 17U, 29U, 16U, 24U}};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 2U)
    {
      return array<arr_t, 1>{arr_t{16U, 42U, 12U, 31U, 16U, 32U, 24U, 21U}};
    }
    if constexpr (std::is_same_v<word_t, uint32_t> && words == 4U)
    {
      return array<arr_t, 2>{arr_t{10U, 11U, 13U, 23U, 6U, 17U, 25U, 18U},
                             arr_t{26U, 21U, 27U, 5U, 20U, 11U, 10U, 20U}};
    }
    if constexpr (std::is_same_v<word_t, uint64_t> && words == 4U)
    {
      return array<arr_t, 2>{arr_t{14U, 52U, 23U, 5U, 25U, 46U, 58U, 32U},
                             arr_t{16U, 57U, 40U, 37U, 33U, 12U, 22U, 32U}};
    }
  }

  static constexpr auto rotations(unsigned i, unsigned round) noexcept
  {
    return rotations_constants().at(i).at(round % 8U);
  }

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

  template <unsigned r>
  static constexpr auto round(auto counter) noexcept
  {
    using qtfy::random::utilities::rotate_left;
    if constexpr (words == 2U)
    {
      counter[0U] += counter[1U];
      counter[1U] = rotate_left<rotations(0U, r)>(counter[1U]) ^ counter[0U];
    }
    if constexpr (words == 4U)
    {
      constexpr auto is_even = (r % 2U) == 0U;
      constexpr size_t index0 = 0U;
      constexpr size_t index1 = is_even ? 1U : 3U;
      constexpr size_t index2 = 2U;
      constexpr size_t index3 = is_even ? 3U : 1U;
      counter[index0] += counter[index1];
      counter[index1] = rotate_left<rotations(0U, r)>(counter[index1]) ^ counter[index0];
      counter[index2] += counter[index3];
      counter[index3] = rotate_left<rotations(1U, r)>(counter[index3]) ^ counter[index2];
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
    result.back() = parity();
    for (size_t i{}; i != words; ++i)
    {
      result[i] = key[i];
      result.back() ^= key[i];
    }
    return result;
  }
};

}  // namespace qtfy::random

#endif
