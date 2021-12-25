#ifndef QTFY_RANDOM_COUNTER_BASED_ENGINE_WITH_LAMBDA_HPP
#define QTFY_RANDOM_COUNTER_BASED_ENGINE_WITH_LAMBDA_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class bijection_t, class bijection_argument_t>
class counter_based_engine_with_bijection
{
  using bijection_result_t =
      std::invoke_result_t<bijection_t, bijection_argument_t>;

  static constexpr auto buffer_size = bijection_result_t{}.size();

  size_t m_index{};
  bijection_result_t m_buffer;
  bijection_argument_t m_counter{};
  bijection_t m_bijection;

 public:
  using result_type = typename bijection_result_t::value_type;

  explicit constexpr counter_based_engine_with_bijection(
      bijection_t bijection) noexcept
      : m_buffer{bijection({})}, m_bijection{bijection}
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
      m_buffer = m_bijection(m_counter);
    }
  }

  constexpr result_type operator()() noexcept
  {
    if (m_index == buffer_size)
    {
      m_buffer = m_bijection(++m_counter);
      m_index = size_t{};
    }
    return m_buffer[m_index++];
  }

  static constexpr result_type max() noexcept
  {
    return std::numeric_limits<result_type>::max();
  }

  static constexpr result_type min() noexcept
  {
    return std::numeric_limits<result_type>::min();
  }
};

template <std::unsigned_integral word_t, size_t words>
constexpr auto create_threefry_engine(counter<word_t, words> key) noexcept
{
  auto functor = threefry_factory<word_t, words, 20, 0, 0>(key);
  using engine_t = counter_based_engine_with_bijection<decltype(functor),
                                                       counter<word_t, words>>;
  return engine_t{functor};
}

template <std::unsigned_integral word_t, size_t words, counter<word_t, words> key>
constexpr auto create_threefry_engine() noexcept
{
  auto functor = threefry_factory<word_t, words, 20, 0, 0, key>();
  using engine_t = counter_based_engine_with_bijection<decltype(functor),
                                                       counter<word_t, words>>;
  return engine_t{functor};
}

template <std::unsigned_integral word_t, size_t words>
constexpr auto create_philox_engine(counter<word_t, words> key) noexcept
{
  auto functor = philox_factory<word_t, words, 10>(key);
  using engine_t = counter_based_engine_with_bijection<decltype(functor),
                                                       counter<word_t, words>>;
  return engine_t{functor};
}

template <std::unsigned_integral word_t, size_t words, counter<word_t, words> key>
constexpr auto create_philox_engine() noexcept
{
  auto functor = philox_factory<word_t, words, 10, key>();
  using engine_t = counter_based_engine_with_bijection<decltype(functor),
                                                       counter<word_t, words>>;
  return engine_t{functor};
}

}  // namespace qtfy::random

#endif
