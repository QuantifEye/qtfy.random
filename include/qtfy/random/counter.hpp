#ifndef QTFY_RANDOM_COUNTER_HPP
#define QTFY_RANDOM_COUNTER_HPP

#include "utlities.hpp"

namespace qtfy::random {

template <class T>
concept counter_word = std::unsigned_integral<T> && !std::same_as<T, bool>;

template <counter_word word_t, size_t words>
class counter;

template <class T>
struct is_counter : public std::false_type
{
};

template <class word_t, size_t words>
struct is_counter<counter<word_t, words>> : public std::true_type
{
};

template <class T>
constexpr bool is_counter_v = is_counter<T>::value;

/**
 * A class that represents a multi word unsigned integer where each word is an
 * unsigned integral. The class shares most of the interface that std::array
 * provides in order to access individual elements. Addition and subtraction is
 * supported for integral numbers which allows the class to be used as a
 * counter.
 *
 * @tparam word_t
 * The type of the individual words that make up the counter.
 * This type has to be an unsigned integral type.
 *
 * @tparam words
 * The number of words that make up the counter.
 *
 * @note
 * The value of the counter can be thought of
 * as counter<uint32_t, 2>{7, 8} == 7 + (8 >> 32)
 * or counter<uint32_t, 2>{7, 8} == 7 + 8 * pow(2, 32)
 *
 * Note that the least significant word is stored in the smallest address
 * location. This implies that {1, 2} + 1 = {2, 2}
 */
template <counter_word word_t, size_t words>
class counter
{
  static_assert(words != 0U, "cannot have a counter with no words");
  using array_t = std::array<word_t, words>;

 public:
  using const_iterator = typename array_t::const_iterator;
  using const_pointer = typename array_t::const_pointer;
  using const_reference = typename array_t::const_reference;
  using const_reverse_iterator = typename array_t::const_reverse_iterator;
  using difference_type = typename array_t::difference_type;
  using iterator = typename array_t::iterator;
  using pointer = typename array_t::pointer;
  using reference = typename array_t::reference;
  using reverse_iterator = typename array_t::reverse_iterator;
  using size_type = typename array_t::size_type;
  using value_type = typename array_t::value_type;

  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  std::array<word_t, words> m_array{};

  constexpr counter &operator++() noexcept;

  constexpr counter &operator--() noexcept;

  template <counter_word T>
  constexpr counter &operator+=(T addend) noexcept;

  template <counter_word T>
  constexpr counter &operator-=(T subtrahend) noexcept;

  constexpr void fill(word_t value) noexcept { m_array.fill(value); }

  constexpr reverse_iterator rbegin() noexcept { return m_array.rbegin(); }

  constexpr const_reverse_iterator rbegin() const noexcept { return m_array.rbegin(); }

  constexpr reverse_iterator rend() noexcept { return m_array.rend(); }

  constexpr const_reverse_iterator rend() const noexcept { return m_array.rend(); }

  constexpr const_reverse_iterator crbegin() const noexcept { return m_array.crbegin(); }

  constexpr const_reverse_iterator crend() const noexcept { return m_array.crend(); }

  constexpr reference operator[](size_type i) noexcept { return m_array[i]; }

  constexpr const_reference operator[](size_type i) const noexcept { return m_array[i]; }

  constexpr reference at(size_type i) { return m_array.at(i); }

  constexpr const_reference at(size_type i) const { return m_array.at(i); }

  constexpr size_type size() const noexcept { return m_array.size(); }

  constexpr size_type max_size() const noexcept { return m_array.max_size(); }

  constexpr bool empty() const noexcept { return words == size_t{}; };

  constexpr iterator begin() noexcept { return m_array.begin(); }

  constexpr iterator end() noexcept { return m_array.end(); }

  constexpr const_iterator begin() const noexcept { return m_array.begin(); }

  constexpr const_iterator end() const noexcept { return m_array.end(); }

  constexpr const_iterator cbegin() const noexcept { return m_array.cbegin(); }

  constexpr const_iterator cend() const noexcept { return m_array.cend(); }

  constexpr pointer data() noexcept { return m_array.data(); }

  constexpr const_pointer data() const noexcept { return m_array.data(); }

  constexpr reference front() noexcept { return m_array.front(); }

  constexpr const_reference front() const noexcept { return m_array.front(); }

  constexpr reference back() noexcept { return m_array.back(); }

  constexpr const_reference back() const noexcept { return m_array.back(); }

  constexpr void swap(counter &right) noexcept { m_array.swap(right.m_array); }
};

template <counter_word word_t, size_t words>
constexpr counter<word_t, words> &counter<word_t, words>::operator++() noexcept
{
  for (size_t i{}; ++m_array[i] == word_t{} && ++i != words;)
  {
  }
  return *this;
}

template <counter_word word_t, size_t words>
constexpr counter<word_t, words> &counter<word_t, words>::operator--() noexcept
{
  constexpr auto max = std::numeric_limits<word_t>::max();
  for (size_t i{}; --m_array[i] == max && ++i != words;)
  {
  };
  return *this;
}

template <counter_word word_t, size_t words>
constexpr void swap(counter<word_t, words> &left, counter<word_t, words> &right) noexcept
{
  left.swap(right);
}

template <counter_word word_t, size_t words>
constexpr bool operator==(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return left.m_array == right.m_array;
}

template <counter_word word_t, size_t words>
constexpr bool operator!=(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return left.m_array != right.m_array;
}

template <bool if_less, bool if_greater, bool if_equal, counter_word word_t, size_t words>
constexpr bool compare_counter(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  for (size_t i{words - 1}; i != (size_t{} - 1U); --i)
  {
    if (left[i] < right[i])
    {
      return if_less;
    }
    if (left[i] > right[i])
    {
      return if_greater;
    }
  }

  return if_equal;
}

template <counter_word word_t, size_t words>
constexpr bool operator<(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return compare_counter<true, false, false>(left, right);
}

template <counter_word word_t, size_t words>
constexpr bool operator>=(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return compare_counter<false, true, true>(left, right);
}

template <counter_word word_t, size_t words>
constexpr bool operator>(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return compare_counter<false, true, false>(left, right);
}

template <counter_word word_t, size_t words>
constexpr bool operator<=(counter<word_t, words> left, counter<word_t, words> right) noexcept
{
  return compare_counter<true, false, true>(left, right);
}

template <counter_word word_t, size_t words, std::unsigned_integral T>
constexpr counter<word_t, words> operator+(counter<word_t, words> left, T addend) noexcept
{
  return left += addend;
}

template <counter_word word_t, size_t words, std::unsigned_integral T>
constexpr counter<word_t, words> operator-(counter<word_t, words> left, T subtrahend) noexcept
{
  return left -= subtrahend;
}

template <counter_word word_t, size_t words>
template <counter_word T>
constexpr counter<word_t, words> &counter<word_t, words>::operator+=(T addend) noexcept
{
  if constexpr (words == 1U)
  {
    m_array[0U] += addend;
  }
  else if constexpr (sizeof(T) <= sizeof(word_t))
  {
    const word_t old_value = m_array[0U];
    const word_t new_value = old_value + static_cast<word_t>(addend);
    m_array[0U] = new_value;
    if (old_value > new_value)
    {
      for (size_t i{1U}; ++m_array[i] == word_t{} && ++i != words;)
      {
      }
    }
  }
  else
  {
    for (size_t i{}; addend && i != words; ++i)
    {
      const word_t old_value = m_array[i];
      const word_t new_value = old_value + static_cast<word_t>(addend);
      m_array[i] = new_value;
      addend >>= std::numeric_limits<word_t>::digits;
      if (new_value < old_value)
      {
        ++addend;
      }
    }
  }
  return *this;
}

template <counter_word word_t, size_t words>
template <counter_word T>
constexpr counter<word_t, words> &counter<word_t, words>::operator-=(T subtrahend) noexcept
{
  if constexpr (words == 1U)
  {
    m_array[0U] -= subtrahend;
  }
  else if constexpr (sizeof(T) <= sizeof(word_t))
  {
    const word_t old_value = m_array[0U];
    const word_t new_value = old_value - static_cast<word_t>(subtrahend);
    m_array[0U] = new_value;
    if (old_value < new_value)
    {
      constexpr auto max = std::numeric_limits<word_t>::max();
      for (size_t i{1U}; --m_array[i] == max && ++i != words;)
      {
      };
    }
  }
  else
  {
    for (size_t i{}; subtrahend && i != words; ++i)
    {
      const word_t old_value = m_array[i];
      const word_t new_value = old_value - static_cast<word_t>(subtrahend);
      m_array[i] = new_value;
      subtrahend >>= std::numeric_limits<word_t>::digits;
      if (new_value > old_value)
      {
        ++subtrahend;
      }
    }
  }
  return *this;
}

template <counter_word new_word_t, counter_word old_word_t, size_t old_count>
requires((sizeof(old_word_t) * old_count) % sizeof(new_word_t) ==
         0U) constexpr auto reinterpret(counter<old_word_t, old_count> old_words) noexcept
{
  constexpr size_t old_word_size = sizeof(old_word_t);
  constexpr size_t new_word_size = sizeof(new_word_t);
  constexpr size_t new_word_count = old_word_size * old_count / new_word_size;
  constexpr size_t new_words_per_old_word = new_word_count / old_count;
  constexpr size_t old_words_per_new_word = old_count / new_word_count;
  constexpr size_t shift =
      new_word_size < old_word_size ? std::numeric_limits<new_word_t>::digits : std::numeric_limits<old_word_t>::digits;

  using return_type = counter<new_word_t, new_word_count>;

  if constexpr (std::is_same_v<old_word_t, new_word_t>)
  {
    return old_words;
  }

  constexpr bool use_bit_cast = true;
  if constexpr (std::endian::native == std::endian::little && use_bit_cast)
  {
    return std::bit_cast<return_type>(old_words);
  }
  else
  {
    return_type new_words{};
    if constexpr (new_word_size < old_word_size)
    {
      for (size_t i{}; i < old_count; ++i)
      {
        for (size_t j{}; j < new_words_per_old_word; ++j)
        {
          new_words[i * old_count + j] = static_cast<new_word_t>(old_words[i] >> (shift * j));
        }
      }
    }
    if constexpr (new_word_size > old_word_size)
    {
      for (size_t i{}; i < new_word_count; ++i)
      {
        for (size_t j{}; j < old_words_per_new_word; ++j)
        {
          new_words[i] += (static_cast<new_word_t>(old_words[i * new_word_count + j]) << (shift * j));
        }
      }
    }
    return new_words;
  }
}



}  // namespace qtfy::random

#endif