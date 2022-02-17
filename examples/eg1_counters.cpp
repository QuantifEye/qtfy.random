#include "qtfy/random.hpp"

auto assert_equal(auto expected, auto actual)
{
  if (expected != actual)
  {
    throw ::std::exception{};
  }
}

template <unsigned power>
requires (power < 32U)
constexpr uint32_t two_to_the_power = uint32_t{1} << power;

int main()
{
  using namespace qtfy::random;

  // counters can be thought of as multi-word positive integers.
  // here we initialise a counter consisting of 4 8-bit unsigned integrals.
  const auto ctr8x4 = counter<uint8_t, 4>{1, 2, 3, 4};

  // we can access the individual elements of the counter as we would access the elements of std::array<uint8_t, 4>.
  // in fact, the counter is implemented as a thin wrapper around a std::array
  assert_equal(1, ctr8x4[0]);
  assert_equal(2, ctr8x4[1]);
  assert_equal(3, ctr8x4[2]);
  assert_equal(4, ctr8x4[3]);

  // the value represented by the counter can be thought of as:
  const uint32_t value = ctr8x4[0] * two_to_the_power<0 * 8> +
                         ctr8x4[1] * two_to_the_power<1 * 8> +
                         ctr8x4[2] * two_to_the_power<2 * 8> +
                         ctr8x4[3] * two_to_the_power<3 * 8>;

  // Another feature of a counter is that it can be reinterpreted as another counter with a different word type.
  // This function will only compile if the number of bits in the old counter is equal to the number of
  // bits in the new counter.

  // for example, reinterpreting counter<uint8_t, 4> as an uint32 counter is ok because
  // we can make an uint32_t counter that has the exact number of bits as the original counter.
  // that is counter<uint32_t, 1> has the same amount of bits as counter<uint8_t, 4>.
  // due to some template calculations, we don't have to think about how many items will be in the new
  // counter as this is calculated for us.
  const auto ctr32x1 = reinterpret<uint32_t>(ctr8x4);

  // note that this value is equivalent to the value calculated manually above.
  assert_equal(ctr32x1[0], value);

  [[maybe_unused]]
  const counter<uint8_t, 3> ctr8x3{1, 2, 3};

  // The following line gives a compile error as the reinterpret function is only implemented
  // for types that can be reinterpreted exactly.

  //  const auto some_counter = reinterpret<uint32_t>(ctr8x3); // error

  return EXIT_SUCCESS;
}