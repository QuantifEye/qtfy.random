#ifndef QTFY_RANDOM_COUNTER_BASED_ENGINE_WITH_LAMBDA_HPP
#define QTFY_RANDOM_COUNTER_BASED_ENGINE_WITH_LAMBDA_HPP

#include "counter.hpp"

namespace qtfy::random {

template <counter_word result_t, class counter_t, class bijection_t>
class counter_based_engine
{
  using bijection_result_t = std::invoke_result_t<bijection_t, counter_t>;
  static_assert(std::is_same_v<bijection_result_t, counter_t>);

  using buffer_t = decltype(reinterpret<result_t>(counter_t{}));
  static constexpr auto buffer_size = buffer_t{}.size();

  size_t m_index;
  buffer_t m_buffer;
  counter_t m_counter;
  [[no_unique_address]] bijection_t m_bijection;

 public:
  constexpr counter_based_engine(counter_t ctr, bijection_t bijection) noexcept
      : m_index{}, m_buffer{reinterpret<result_t>(bijection(ctr))}, m_counter{ctr}, m_bijection{bijection}
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
      m_buffer = reinterpret<result_t>(m_bijection(m_counter));
    }
  }

  constexpr result_t operator()() noexcept
  {
    if (m_index == buffer_size)
    {
      m_buffer = reinterpret<result_t>(m_bijection(++m_counter));
      m_index = size_t{};
    }

    return m_buffer[m_index++];
  }

  static constexpr result_t max() noexcept { return std::numeric_limits<result_t>::max(); }

  static constexpr result_t min() noexcept { return std::numeric_limits<result_t>::min(); }

 private:
  template <class main_t, class sub_t>
  struct alignas(alignof(main_t)) alias
  {
    static_assert(sizeof(main_t) / sizeof(sub_t) != 0U);
    static_assert(sizeof(main_t) % sizeof(sub_t) == 0U);
    sub_t parts[sizeof(main_t) / sizeof(sub_t)];
  };

 public:
  template <size_t required_bits>
  requires(required_bits <= 64) static consteval auto init_bits() noexcept
  {
    if constexpr (required_bits <= 8)
    {
      return uint8_t{};
    }
    if constexpr (required_bits <= 16)
    {
      return uint16_t{};
    }
    if constexpr (required_bits <= 32)
    {
      return uint32_t{};
    }
    if constexpr (required_bits <= 64)
    {
      return uint64_t{};
    }
  }

  template <size_t required_bits>
  constexpr auto get_bits() noexcept requires(required_bits <= 64)
  {
    using bits_t = decltype(init_bits<required_bits>());

    constexpr size_t bits_per_draw = std::numeric_limits<result_t>::digits;
    constexpr size_t required_draws =
        required_bits % bits_per_draw == 0U ? required_bits / bits_per_draw : required_bits / bits_per_draw + 1;
    constexpr size_t shift = required_draws * bits_per_draw - required_bits;

    if constexpr (required_bits <= bits_per_draw)
    {
      return static_cast<bits_t>(operator()() >> shift);
    }
    else if constexpr (std::endian::native == std::endian::little)
    {
      alias<bits_t, result_t> a{};
      for (size_t i{}; i < required_draws; ++i)
      {
        a.parts[i] = operator()();
      }
      return std::bit_cast<bits_t>(a) >> shift;
    }
    else
    {
      bits_t result = operator()();
      for (size_t i{1}; i < required_draws; ++i)
      {
        result <<= bits_per_draw;
        result += operator()();
      }

      return result >> shift;
    }
  }

  template <std::floating_point T = double, unsigned bits = std::numeric_limits<T>::digits>
  requires(bits <= 64) constexpr T next_canonical() noexcept
  {
    constexpr auto digits = std::numeric_limits<T>::digits;
    constexpr int scale = bits <= digits ? bits : digits;
    return std::scalbn(static_cast<T>(get_bits<scale>()), -scale);
  }
};

template <counter_word result_t, class counter_t, class bijection_t>
constexpr counter_based_engine<result_t, counter_t, bijection_t> make_counter_based_engine(
    counter_t ctr, bijection_t bijection) noexcept
{
  return {ctr, bijection};
}

template <counter_word word_t, size_t words, unsigned rounds, counter_word result_t = word_t,
          threefry_constants<word_t, words> constants = default_threefry_constants<word_t, words>()>
constexpr auto make_threefry_engine(counter<word_t, words> ctr, counter<word_t, words> key) noexcept
{
  return make_counter_based_engine<result_t>(ctr, make_threefry_functor<word_t, words, rounds, constants>(key));
}

template <counter_word word_t, size_t words, unsigned rounds, counter<word_t, words> key,
          counter_word result_t = word_t,
          threefry_constants<word_t, words> constants = default_threefry_constants<word_t, words>()>
constexpr auto make_threefry_engine(counter<word_t, words> ctr) noexcept
{
  return make_counter_based_engine<result_t>(ctr, make_threefry_functor<word_t, words, rounds, key, constants>());
}

template <counter_word word_t, size_t words, unsigned rounds, counter_word result_t = word_t,
          philox_constants<word_t, words> constants = default_philox_constants<word_t, words>()>
constexpr auto make_philox_engine(counter<word_t, words> ctr, counter<word_t, words / 2U> key) noexcept
{
  return make_counter_based_engine<result_t>(ctr, make_philox_functor<word_t, words, rounds, constants>(key));
}

template <counter_word word_t, size_t words, unsigned rounds, counter<word_t, words / 2U> key,
          counter_word result_t = word_t,
          philox_constants<word_t, words> constants = default_philox_constants<word_t, words>()>
constexpr auto make_philox_engine(counter<word_t, words> ctr) noexcept
{
  return make_counter_based_engine<result_t>(ctr, make_philox_functor<word_t, words, rounds, key, constants>());
}

}  // namespace qtfy::random

#endif
