#ifndef QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP
#define QTFY_RANDOM_COUNTER_BASED_ENGINE_HPP

#include "counter.hpp"

namespace qtfy::random {

template <class trait_t, class result_t = typename trait_t::word_type>
class counter_based_engine
{
 public:
  using word_type = typename trait_t::word_type;
  using key_type = typename trait_t::key_type;
  using counter_type = typename trait_t::counter_type;
  using internal_key_type = typename trait_t::internal_key_type;
  using buffer_type = decltype(reinterpret<result_t>(trait_t::bijection({}, {})));

 private:
  static constexpr size_t buffer_size = buffer_type{}.size();
  size_t m_index{};
  buffer_type m_buffer{};
  counter_type m_counter{};
  const internal_key_type m_key{};

 public:
  static constexpr internal_key_type set_key(key_type key) { return trait_t::set_key(key); }

  static constexpr buffer_type bijection(counter_type counter, internal_key_type internal_key) noexcept
  {
    return reinterpret<result_t>(trait_t::bijection(counter, internal_key));
  }

  constexpr counter_based_engine(key_type key, counter_type counter) noexcept
      : m_index{}, m_buffer{}, m_counter{counter}, m_key{set_key(key)}
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
