#ifndef QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP
#define QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class trait_t,
          std::unsigned_integral result_t = typename trait_t::word_type>
class counter_based_engine
{
 public:
  using word_type = typename trait_t::word_type;
  using key_type = typename trait_t::key_type;
  using counter_type = typename trait_t::counter_type;
  using internal_key_type = typename trait_t::internal_key_type;
  using buffer_type =
      decltype(reinterpret<result_t>(trait_t::bijection({}, {})));

 private:
  static constexpr size_t buffer_size = buffer_type{}.size();
  size_t m_index{};
  buffer_type m_buffer{};
  counter_type m_counter{};
  const internal_key_type m_key{};

 public:
  static constexpr internal_key_type set_key(key_type key) noexcept
  {
    return trait_t::set_key(key);
  }

  static constexpr buffer_type bijection(
      counter_type counter, internal_key_type internal_key) noexcept
  {
    return reinterpret<result_t>(trait_t::bijection(counter, internal_key));
  }

  constexpr counter_based_engine(key_type key, counter_type counter) noexcept
      : m_index{}, m_buffer{}, m_counter{counter}, m_key{set_key(key)}
  {
    m_buffer = bijection(m_counter, m_key);
  }

  explicit constexpr counter_based_engine(key_type key) noexcept
      : counter_based_engine(key, counter_type{})
  {
  }

  constexpr counter_based_engine() noexcept
      : counter_based_engine(key_type{}, counter_type{})
  {
  }

  constexpr void discard(unsigned long long steps) noexcept
  {
    auto chunks = steps / buffer_size;
    m_index += steps - chunks * buffer_size;
    if (m_index >= buffer_size)
    {
      m_index -= buffer_size;
      ++chunks;
    }
    if (chunks != 0U)
    {
      m_counter += chunks;
      m_buffer = bijection(m_counter, m_key);
    }
  }

  constexpr result_t operator()() noexcept
  {
    if (m_index == buffer_size)
    {
      m_buffer = bijection(++m_counter, m_key);
      m_index = size_t{};
    }
    return m_buffer[m_index++];
  }

  static constexpr result_t max() noexcept
  {
    return std::numeric_limits<result_t>::max();
  }

  static constexpr result_t min() noexcept
  {
    return std::numeric_limits<result_t>::min();
  }

 private:
  template <size_t required_bits>
  static consteval auto init_int(std::unsigned_integral auto x) noexcept
  {
    if constexpr (required_bits <= 8)
    {
      return uint8_t{x};
    }
    if constexpr (required_bits <= 16)
    {
      return uint16_t{x};
    }
    if constexpr (required_bits <= 32)
    {
      return uint32_t{x};
    }
    if constexpr (required_bits <= 64)
    {
      return uint64_t{x};
    }
  }

  template <std::unsigned_integral main_t, std::unsigned_integral sub_t>
  struct alignas(alignof(main_t)) alias
  {
    static_assert(sizeof(main_t) >= sizeof(sub_t));
    static_assert(sizeof(main_t) / sizeof(sub_t) != 0);
    static_assert(sizeof(main_t) % sizeof(sub_t) == 0);

    static constexpr size_t size = sizeof(main_t) / sizeof(sub_t);
    sub_t parts[size];
  };

  template <size_t required_bits>
  requires(required_bits <= 64) constexpr auto get_bits() noexcept
  {
    using return_t = decltype(init_int<required_bits>(0U));
    constexpr size_t bits_per_draw = std::numeric_limits<result_t>::digits;
    constexpr size_t required_draws = required_bits % bits_per_draw == 0U
                                          ? required_bits / bits_per_draw
                                          : required_bits / bits_per_draw + 1;

    constexpr size_t shift = required_draws * bits_per_draw - required_bits;
    if constexpr (required_bits <= bits_per_draw)
    {
      return operator()() >> shift;
    }

    constexpr bool experimental = true;
    if constexpr (std::endian::native == std::endian::little && experimental)
    {
      using sub_t = decltype(operator()());
      using alias_t = alias<return_t, sub_t>;
      alias_t a{};
      for (size_t i{}; i < required_draws; ++i)
      {
        a.parts[i] = operator()();
      }
      return std::bit_cast<return_t>(a) >> shift;
    }
    else
    {
      auto result = init_int<required_bits>(operator()());
      for (size_t i{1}; i < required_draws; ++i)
      {
        result <<= bits_per_draw;
        result += operator()();
      }

      return result >> shift;
    }
  }

 public:
  template <std::floating_point T = double,
            unsigned bits = std::numeric_limits<T>::digits>
  constexpr T next_canonical() noexcept
  {
    constexpr auto digits = std::numeric_limits<T>::digits;
    constexpr int scale = bits <= digits ? bits : digits;
    return std::scalbn(static_cast<T>(get_bits<scale>()), -scale);
  }
};

}  // namespace qtfy::random

#endif
