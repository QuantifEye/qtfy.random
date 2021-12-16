#ifndef QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP
#define QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class Trait, class result_t = typename Trait::word_type>
class counter_based_engine
{
 public:
  using word_type = typename Trait::word_type;
  using key_type = typename Trait::key_type;
  using counter_type = typename Trait::counter_type;
  using internal_key_type = typename Trait::internal_key_type;

 private:
  static constexpr auto bijection(counter_type counter, internal_key_type internal_key) noexcept
  {
    if constexpr (std::is_same_v<word_type, result_t>)
    {
      return Trait::bijection(counter, internal_key);
    }
    else
    {
      return reinterpret<result_t>(Trait::bijection(counter, internal_key));
    }
  }

  using buffer_t = decltype(bijection({}, {}));
  static constexpr size_t buffer_size = buffer_t{}.size();
  buffer_t m_buffer{};
  size_t m_index{};
  counter_type m_counter{};
  internal_key_type m_key{};

 public:
  constexpr counter_based_engine(key_type key, counter_type counter) noexcept
      : m_buffer{}, m_index{}, m_counter{counter}, m_key{Trait::set_key(key)}
  {
    m_buffer = bijection(m_counter, m_key);
  }

  explicit constexpr counter_based_engine(key_type key) noexcept : counter_based_engine(key, counter_type{}) {}

  constexpr counter_based_engine() noexcept : counter_based_engine(key_type{}, counter_type{}) {}

  constexpr void discard(unsigned long long steps) noexcept
  {
    auto chunks = steps / buffer_size;
    m_index += steps - chunks * buffer_size;
    if (m_index >= buffer_size)
    {
      m_index -= buffer_size;
      ++chunks;
    }
    if (chunks)
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

  static constexpr result_t max() noexcept { return std::numeric_limits<result_t>::max(); }

  static constexpr result_t min() noexcept { return std::numeric_limits<result_t>::max(); }
};

}  // namespace qtfy::random

#endif
