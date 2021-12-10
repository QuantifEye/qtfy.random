#include <array>
#include <bit>
#include <cinttypes>
#include <concepts>
#include <cstring>
#include <limits>
#include <type_traits>

#include <cmath>

namespace std {

template <class TTo, class TFrom>
static constexpr auto bit_cast(TFrom value) noexcept
{
  // TODO: delete once std::bit_cast is available.
  static_assert(sizeof(TTo) == sizeof(TFrom));
  TTo result;
  std::memcpy(&result, &value, sizeof(TTo));
  return result;
}

}  // namespace std

namespace qtfy::random {

/**
 * A class that represents a multi word unsigned integer where each word is an unsigned integral.
 * The class shares most of the interface that std::array provides in order to access indivisual elements.
 * Addition and subtraction is supported for integral numbers which allows the
 * class to be used as a counter.
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
 * as Counter<uint32_t, 2>{7, 8} == 7 + 8 >> 32
 * or Counter<uint32_t, 2>{7, 8} == 7 + 8 * pow(2, 32)
 *
 * Note that the least significant word is stored in the smallest address location.
 * This implies that {1, 2} + 1 = {2, 2}
 */
template <class word_t, size_t words>
class Counter
{
  // TODO: most (maybe all) use cases for this class will only have a few words.
  // TODO: thus it is important to make sure that loops unroll for performance reasons

  static_assert(std::is_integral_v<word_t> && std::is_unsigned_v<word_t>);
  static_assert(words != 0U);

  // adds an unsigned integer to this.
  // for cases where sizeof(T) <= sizeof(word_t)
  template <std::unsigned_integral T>
  constexpr Counter &add_small(T addend) noexcept;

  // adds an unsigned integer to this.
  // for cases where sizeof(T) > sizeof(word_t)
  template <std::unsigned_integral T>
  constexpr Counter &add_large(T addend) noexcept;

  // subtracts an unsigned integer from this
  // for cases where sizeof(T) <= sizeof(word_t)
  template <std::unsigned_integral T>
  constexpr Counter &subtract_small(T subtrahend) noexcept;

  // subtracts an unsigned integer from this
  // for cases where sizeof(T) > sizeof(word_t)
  template <std::unsigned_integral T>
  constexpr Counter &subtract_large(T subtrahend) noexcept;

  template <std::integral T>
  constexpr Counter &add(T addend) noexcept;

  template <std::integral T>
  constexpr Counter &subtract(T subtrahend) noexcept;

  // rolls the counter forward from the starting index
  template <size_t start_index>
  constexpr void increment() noexcept;

  // rolls the counter back from the starting index
  template <size_t start_index>
  constexpr void decrement() noexcept;

 public:
  using array_t = std::array<word_t, words>;

  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  array_t m_array{};

  /**
   * The greatest possible value the the type can have.
   */
  static constexpr Counter max() noexcept;

  /**
   * The smallest possible value the the type can have.
   */
  static constexpr Counter min() noexcept;

  template<std::unsigned_integral new_word_t>
  constexpr auto reinterpret()
  {
    constexpr auto bytes = sizeof(word_t) * words;
    static_assert(bytes % sizeof(new_word_t) == 0);
    static_assert(std::endian::native == std::endian::little, "currently only supported for little endian");
    if constexpr (std::endian::native == std::endian::little)
    {
      return std::bit_cast<Counter<new_word_t, bytes / sizeof(new_word_t)>>(*this);
    }

    // TODO: for this to be implemented on big endian machines as a bit cast, the words in the array would have to be in
    // reverse order, that is, most significant word in position 0
  }


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

  constexpr void fill(word_t value) noexcept
  {
    // TODO: this is implemented manually to ensure it is noexcept and constexpr
    for (auto &x : m_array)
    {
      x = value;
    }
  }

  static constexpr size_t word_count() noexcept { return words; }

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

  constexpr Counter &operator++() noexcept;

  constexpr Counter &operator--() noexcept;

  template <std::integral T>
  constexpr Counter &operator+=(T addend) noexcept;

  template <std::integral T>
  constexpr Counter &operator-=(T subtrahend) noexcept;

  constexpr void swap(Counter &right) noexcept { m_array.swap(right.m_array); }
};

template <class word_t, size_t words>
constexpr void swap(Counter<word_t, words> &left, Counter<word_t, words> &right) noexcept
{
  left.swap(right);
}

template <class word_t, size_t words>
static constexpr bool operator==(Counter<word_t, words> left, Counter<word_t, words> right) noexcept
{
  return left.m_array == right.m_array;
}

template <class word_t, size_t words>
static constexpr bool operator!=(Counter<word_t, words> left, Counter<word_t, words> right) noexcept
{
  return left.m_array != right.m_array;
}

template <class word_t, size_t words, std::integral TAddend>
static constexpr Counter<word_t, words> operator+(Counter<word_t, words> left, TAddend addend) noexcept
{
  return left += addend;
}

template <class word_t, size_t words, std::integral TAddend>
static constexpr Counter<word_t, words> operator-(Counter<word_t, words> left, TAddend subtrahend) noexcept
{
  return left -= subtrahend;
}

template <class word_t, size_t words>
template <size_t start_index>
constexpr void Counter<word_t, words>::increment() noexcept
{
  static_assert(start_index < words);
  static_assert(words != 0);

  // TODO: This is an attempt to be terse and minimal, should this be made simpler?
  // TODO: Should this be a for loop?
  // TODO: Will this unroll just as well as a for loop with the "i" being outside the loop?
  for (size_t i{start_index}; ++m_array[i] == std::numeric_limits<word_t>::min() && ++i != words;)
  {
  }
}

template <class word_t, size_t words>
template <size_t start_index>
constexpr void Counter<word_t, words>::decrement() noexcept
{
  // TODO: This is an attempt to be terse and minimal, should this be made simpler?
  // TODO: Should this be a for loop?
  // TODO: Will this unroll just as well as a for loop with the "i" being outside the loop?
  static_assert(start_index < words);
  static_assert(words != 0);
  for (size_t i{start_index}; --m_array[i] == std::numeric_limits<word_t>::max() && ++i != words;)
  {
  };
}

template <class word_t, size_t words>
template <std::unsigned_integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::add_small(T addend) noexcept
{
  const word_t old_value = m_array[0];
  const word_t new_value = old_value + static_cast<word_t>(addend);
  m_array[0] = new_value;
  if (old_value > new_value)
  {
    increment<1>();
  }
  return *this;
}

template <class word_t, size_t words>
template <std::unsigned_integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::add_large(T addend) noexcept
{
  // TODO: Should this be a for loop? Will this unroll just as well as a for loop with the "index" being outside the
  // loop?
  size_t index{};
  do
  {
    const word_t current_word = m_array[index];
    const word_t new_word = current_word + static_cast<word_t>(addend);
    m_array[index] = new_word;
    constexpr unsigned shift = std::numeric_limits<word_t>::digits;
    addend >>= shift;
    if (new_word < current_word)
    {
      ++addend;
    }
  } while (addend && ++index != words);
  return *this;
}

template <class word_t, size_t words>
template <std::unsigned_integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::subtract_small(T subtrahend) noexcept
{
  const word_t old_value = m_array[0];
  const word_t new_value = old_value - static_cast<word_t>(subtrahend);
  m_array[0] = new_value;
  if (old_value < new_value)
  {
    decrement<1>();
  }
  return *this;
}

template <class word_t, size_t words>
template <std::unsigned_integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::subtract_large(T subtrahend) noexcept
{
  // TODO: Should this be a for loop?
  // TODO: Will this unroll just as well as a for loop with the "index" being outside the loop?
  size_t index{};
  do
  {
    const word_t current_word = m_array[index];
    const word_t new_word = current_word - static_cast<word_t>(subtrahend);
    m_array[index] = new_word;
    subtrahend >>= std::numeric_limits<word_t>::digits;
    if (new_word > current_word)
    {
      ++subtrahend;
    }
  } while (subtrahend && ++index == words);
  return *this;
}

template <class word_t, size_t words>
template <std::integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::add(T addend) noexcept
{
  static_assert(std::is_integral_v<T>);
  using unsigned_t = std::make_unsigned_t<T>;
  if constexpr (std::is_signed_v<T>)
  {
    if (addend < T{0})
    {
      if (addend == std::numeric_limits<T>::min())
      {
        return subtract(unsigned_t{std::numeric_limits<T>::max()} + unsigned_t{1});
      }
      else
      {
        return subtract(static_cast<unsigned_t>(-addend));
      }
    }
    else
    {
      return add(static_cast<unsigned_t>(addend));
    }
  }
  else if constexpr (sizeof(T) <= sizeof(word_t))
  {
    return add_small(static_cast<word_t>(addend));
  }
  else
  {
    if (addend <= std::numeric_limits<word_t>::max())
    {
      return add_small(static_cast<word_t>(addend));
    }
    else
    {
      return add_large(addend);
    }
  }
}

template <class word_t, size_t words>
template <std::integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::subtract(T subtrahend) noexcept
{
  static_assert(std::is_integral_v<T>);
  using unsigned_t = std::make_unsigned_t<T>;
  if constexpr (std::is_signed_v<T>)
  {
    if (subtrahend < T{0})
    {
      if (subtrahend == std::numeric_limits<T>::min())
      {
        return add(unsigned_t{std::numeric_limits<T>::max()} + unsigned_t{1});
      }
      else
      {
        return add(static_cast<unsigned_t>(-subtrahend));
      }
    }
    else
    {
      return subtract(static_cast<unsigned_t>(subtrahend));
    }
  }
  else if constexpr (sizeof(T) <= sizeof(word_t))
  {
    return subtract_small(static_cast<word_t>(subtrahend));
  }
  else
  {
    if (subtrahend <= std::numeric_limits<word_t>::max())
    {
      return subtract_small(static_cast<word_t>(subtrahend));
    }
    else
    {
      return subtract_large(subtrahend);
    }
  }
}

template <class word_t, size_t words>
constexpr Counter<word_t, words> &Counter<word_t, words>::operator++() noexcept
{
  increment<0>();
  return *this;
}

template <class word_t, size_t words>
constexpr Counter<word_t, words> &Counter<word_t, words>::operator--() noexcept
{
  decrement<0>();
  return *this;
}

template <class word_t, size_t words>
template <std::integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::operator+=(T addend) noexcept
{
  return add<std::remove_cvref_t<T>>(addend);
}

template <class word_t, size_t words>
template <std::integral T>
constexpr Counter<word_t, words> &Counter<word_t, words>::operator-=(T subtrahend) noexcept
{
  return subtract<std::remove_cvref_t<T>>(subtrahend);
}

template <class word_t, size_t words>
constexpr Counter<word_t, words> Counter<word_t, words>::max() noexcept
{
  Counter<word_t, words> result{};
  result.fill(std::numeric_limits<word_t>::max());
  return result;
}

template <class word_t, size_t words>
constexpr Counter<word_t, words> Counter<word_t, words>::min() noexcept
{
  Counter<word_t, words> result{};
  result.fill(std::numeric_limits<word_t>::min());
  return result;
}

template <class word_t, unsigned words, unsigned rounds>
class Philox
{
 public:
  using counter_t = Counter<word_t, words>;
  using internal_key_t = Counter<word_t, words / 2U>;
  using key_t = Counter<word_t, words / 2U>;
  using result_type = word_t;

 private:
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<result_type, uint32_t> || std::is_same_v<result_type, uint64_t>);
  static_assert(rounds <= 16U);

  template <class T, unsigned w>
  static constexpr bool is_case() noexcept
  {
    return std::is_same_v<result_type, T> && words == w;
  }

  static constexpr auto bump_constants() noexcept
  {
    if constexpr (is_case<uint32_t, 2U>())
    {
      return std::array<uint32_t, 1U>{0x9E3779B9};
    }
    if constexpr (is_case<uint32_t, 4U>())
    {
      return std::array<uint32_t, 2U>{0x9E3779B9, 0xBB67AE85};
    }
    if constexpr (is_case<uint64_t, 2U>())
    {
      return std::array<uint64_t, 1U>{0x9E3779B97F4A7C15};
    }
    if constexpr (is_case<uint64_t, 4U>())
    {
      return std::array<uint64_t, 2U>{0x9E3779B97F4A7C15, 0xBB67AE8584CAA73B};
    }
  }

  static constexpr auto multipliers_constants() noexcept
  {
    if constexpr (is_case<uint32_t, 2U>())
    {
      return std::array<uint64_t, 1U>{0xD256D193};
    }
    if constexpr (is_case<uint32_t, 4U>())
    {
      return std::array<uint64_t, 2U>{0xD2511F53, 0xCD9E8D57};
    }
    if constexpr (is_case<uint64_t, 2U>())
    {
      return std::array<uint64_t, 1U>{0xD2B74407B1CE6E93};
    }
    if constexpr (is_case<uint64_t, 4U>())
    {
      return std::array<uint64_t, 2U>{0xD2E7470EE14C6C93, 0xCA5A826395121157};
    }
  }

  struct HiLo
  {
    result_type lo;
    result_type hi;
  };

  struct BigEndianHiLo
  {
    result_type hi;
    result_type lo;
  };

  template <class TProduct>
  static constexpr auto hilo_cast(TProduct product) noexcept
  {
    static_assert(sizeof(TProduct) / 2 == sizeof(result_type));
    if constexpr (std::endian::native == std::endian::little)
    {
      return std::bit_cast<HiLo>(product);
    }
    if constexpr (std::endian::native == std::endian::big)
    {
      return std::bit_cast<BigEndianHiLo>(product);
    }
    else
    {
      return HiLo{static_cast<result_type>(product), static_cast<result_type>(product >> sizeof(result_type))};
    }
  }

  static constexpr bool has_gcc_int128() noexcept
  {
#if defined(__GNUC__) && defined(__x86_64__)
    return true;
#else
    return false;
#endif
  }

  template <uint64_t left>
  static constexpr auto big_mul(result_type right) noexcept
  {
    if constexpr (std::is_same_v<result_type, uint32_t>)
    {
      return hilo_cast(left * right);
    }
    if constexpr (std::is_same_v<result_type, uint64_t>)
    {
      if constexpr (has_gcc_int128())
      {
        return hilo_cast(static_cast<unsigned __int128>(left) * right);
      }
      else
      {
        constexpr uint64_t lower_bits = std::numeric_limits<uint32_t>::max();
        constexpr uint64_t a_low = left & lower_bits;
        constexpr uint64_t a_high = left >> 32U;
        const uint64_t b_low = right & lower_bits;
        const uint64_t b_high = right >> 32U;
        const uint64_t t = a_high * b_low + (a_low * b_low >> 32U);
        const uint64_t tl = a_low * b_high + (t & lower_bits);
        return HiLo{left * right, a_high * b_high + (t >> 32U) + (tl >> 32U)};
      }
    }
  }

  static constexpr counter_t round(counter_t ctr, internal_key_t key) noexcept
  {
    constexpr auto multipliers = multipliers_constants();
    if constexpr (words == 2U)
    {
      const auto product = big_mul<multipliers[0U]>(ctr[0U]);
      return {product.hi ^ key[0U] ^ ctr[1U], product.lo};
    }
    if constexpr (words == 4U)
    {
      const auto product0 = big_mul<multipliers[0U]>(ctr[0U]);
      const auto product1 = big_mul<multipliers[1U]>(ctr[2U]);
      return {product1.hi ^ ctr[1U] ^ key[0U], product1.lo, product0.hi ^ ctr[3U] ^ key[1U], product0.lo};
    }
  }

  static constexpr internal_key_t bump_key(internal_key_t key) noexcept
  {
    constexpr auto bumps = bump_constants();
    if constexpr (words == 2U)
    {
      key[0U] += bumps[0U];
    }
    if constexpr (words == 4U)
    {
      key[0U] += bumps[0U];
      key[1U] += bumps[1U];
    }
    return key;
  }

 public:
  static constexpr internal_key_t set_key(key_t key) noexcept { return key; }

  static constexpr counter_t bijection(counter_t counter, internal_key_t key) noexcept
  {
    if constexpr (rounds != 0U)
    {
      counter = round(counter, key);
      for (unsigned r{rounds}; --r;)
      {
        key = bump_key(key);
        counter = round(counter, key);
      }
    }
    return counter;
  }
};

template <class word_t, unsigned words, unsigned rounds>
class ThreeFry
{
  static_assert(words == 2U || words == 4U);
  static_assert(std::is_same_v<word_t, uint32_t> || std::is_same_v<word_t, uint64_t>);

  // TODO: not sure if this restriction is necessary.
  static_assert(rounds <= 72U);

 public:
  using counter_t = Counter<word_t, words>;
  using key_t = Counter<word_t, words>;
  using internal_key_t = Counter<word_t, words + 1U>;
  using result_type = word_t;

 private:
  static constexpr auto parity() noexcept
  {
    if constexpr (std::is_same_v<result_type, uint32_t>)
    {
      return static_cast<uint32_t>(0x1BD11BDA);
    }
    if constexpr (std::is_same_v<result_type, uint64_t>)
    {
      return static_cast<uint64_t>(0x1BD11BDAA9FC1A22);
    }
  }

  template <class T, unsigned w>
  static constexpr bool is_case() noexcept
  {
    return std::is_same_v<result_type, T> && words == w;
  }

  template <unsigned round>
  static constexpr auto rotations() noexcept
  {
    constexpr unsigned result_size = 8U;
    constexpr unsigned index = round % result_size;
    using arr = std::array<unsigned, result_size>;
    if constexpr (is_case<uint32_t, 2U>())
    {
      return arr{13U, 15U, 26U, 6U, 17U, 29U, 16U, 24U}[index];
    }
    if constexpr (is_case<uint64_t, 2U>())
    {
      return arr{16U, 42U, 12U, 31U, 16U, 32U, 24U, 21U}[index];
    }
    if constexpr (is_case<uint32_t, 4U>())
    {
      constexpr arr rotation0{10U, 11U, 13U, 23U, 6U, 17U, 25U, 18U};
      constexpr arr rotation1{26U, 21U, 27U, 5U, 20U, 11U, 10U, 20U};
      return std::array<unsigned, 2>{rotation0[index], rotation1[index]};
    }
    if constexpr (is_case<uint64_t, 4>())
    {
      constexpr arr rotation0{14U, 52U, 23U, 5U, 25U, 46U, 58U, 32U};
      constexpr arr rotation1{16U, 57U, 40U, 37U, 33U, 12U, 22U, 32U};
      return std::array<unsigned, 2>{rotation0[index], rotation1[index]};
    }
  }

  /**
   * To ensure that shift is always a compile time constant
   */
  template <unsigned shift>
  static constexpr result_type rotate_left(result_type word) noexcept
  {
    constexpr unsigned digits = std::numeric_limits<result_type>::digits;
    constexpr unsigned left_shift = shift % digits;
    constexpr unsigned right_shift = digits - left_shift;
    if constexpr (left_shift != 0U)
    {
      return (word << left_shift) | (word >> right_shift);
    }
    else
    {
      return word;
    }
  }

  template <unsigned r>
  static constexpr counter_t bump_counter(counter_t counter, internal_key_t key) noexcept
  {
    constexpr size_t internal_key_size = internal_key_t::word_count();
    constexpr result_type b = (r + 1U) / 4U;
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

  template <unsigned r, internal_key_t key>
  static constexpr counter_t bump_counter_with_known_key(counter_t counter) noexcept
  {
    constexpr size_t internal_key_size = internal_key_t::word_count();
    constexpr result_type b = (r + 1U) / 4U;
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
  static constexpr counter_t round(counter_t counter) noexcept
  {
    constexpr auto rotation = rotations<r>();
    if constexpr (words == 2U)
    {
      counter[0U] += counter[1U];
      counter[1U] = rotate_left<rotation>(counter[1U]) ^ counter[0U];
    }
    if constexpr (words == 4U)
    {
      constexpr auto is_even = (r % 2U) == 0U;
      constexpr size_t index0 = 0U;
      constexpr size_t index1 = is_even ? 1U : 3U;
      constexpr size_t index2 = 2U;
      constexpr size_t index3 = is_even ? 3U : 1U;
      counter[index0] += counter[index1];
      counter[index1] = rotate_left<rotation[0U]>(counter[index1]) ^ counter[index0];
      counter[index2] += counter[index3];
      counter[index3] = rotate_left<rotation[1U]>(counter[index3]) ^ counter[index2];
    }
    return counter;
  }

  template <unsigned r>
  static constexpr counter_t round_applier(counter_t counter, internal_key_t key) noexcept
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
  static constexpr counter_t bijection(counter_t counter, internal_key_t key) noexcept
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

  static constexpr internal_key_t set_key(key_t key) noexcept
  {
    internal_key_t result{};
    result.back() = parity();
    for (size_t i{}; i != words; ++i)
    {
      result[i] = key[i];
      result.back() ^= key[i];
    }
    return result;
  }
};

template <class Trait>
class CounterBasedGenerator
{
 public:
  using result_type = typename Trait::result_type;
  using key_t = typename Trait::key_t;
  using counter_t = typename Trait::counter_t;
  using internal_key_t = typename Trait::internal_key_t;

 private:
  static constexpr size_t buffer_size = counter_t::word_count();
  counter_t m_buffer{};
  size_t m_index{};
  counter_t m_counter{};
  internal_key_t m_key{};

 public:
  static constexpr counter_t bijection(counter_t counter, internal_key_t internal_key) noexcept
  {
    return Trait::bijection(counter, internal_key);
  }

  static constexpr internal_key_t set_key(key_t key) noexcept { return Trait::set_key(key); }

  constexpr CounterBasedGenerator(key_t key, counter_t counter) noexcept
      : m_buffer{}, m_index{}, m_counter{counter}, m_key{set_key(key)}
  {
    m_buffer = bijection(m_counter, m_key);
  }

  explicit constexpr CounterBasedGenerator(key_t key) noexcept : CounterBasedGenerator(key, counter_t{}) {}

  constexpr CounterBasedGenerator() noexcept : CounterBasedGenerator(key_t{}, counter_t{}) {}

  template <std::unsigned_integral T>
  constexpr void discard(T steps) noexcept
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

  constexpr result_type operator()() noexcept
  {
    if (m_index == buffer_size)
    {
      m_buffer = bijection(++m_counter, m_key);
      m_index = size_t{0};
    }
    return m_buffer[m_index++];
  }

  static constexpr result_type max() noexcept { return std::numeric_limits<result_type>::max(); }

  static constexpr result_type min() noexcept { return std::numeric_limits<result_type>::max(); }
};

template <class word_t, unsigned words, unsigned rounds>
using ThreeFryGenerator = CounterBasedGenerator<ThreeFry<word_t, words, rounds>>;

template <class word_t, unsigned words, unsigned rounds>
using PhiloxGenerator = CounterBasedGenerator<Philox<word_t, words, rounds>>;
}  // namespace qtfy::random