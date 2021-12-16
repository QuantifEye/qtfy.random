#include "qtfy/random.hpp"
#include "test_tools.hpp"

using namespace qtfy::random;

void test_increment_decrement()
{
  auto test = [](auto input, auto expected_incremented) {
    auto t1 = input;
    auto t2 = expected_incremented;
    assert_are_equal(++t1, expected_incremented);
    assert_are_equal(--t2, input);
  };

  using ctr2 = counter<uint32_t, 2>;
  test(ctr2{1U, 1U}, ctr2{2U, 1U});
  test(ctr2{0U, 2U}, ctr2{1U, 2U});
  test(ctr2{0U, 0U}, ctr2{1U, 0U});
  test(ctr2{UINT32_MAX, UINT32_MAX}, ctr2{0U, 0U});
  test(ctr2{UINT32_MAX, 2U}, ctr2{0U, 3U});
}

void test_add_small()
{
  constexpr uint32_t max32 = std::numeric_limits<uint32_t>::max();
  constexpr uint32_t two = 2;
  using ctr2 = counter<uint32_t, 2U>;
  {
    ctr2 actual = ctr2{max32, 1U} + two;
    ctr2 expected{1U, 2U};
    assert_are_equal(actual, expected);
  }
  {
    ctr2 actual = ctr2{2U, 1U} + two;
    ctr2 expected{4U, 1U};
    assert_are_equal(actual, expected);
  }
}

void test_add_large()
{
  constexpr uint32_t max32 = std::numeric_limits<uint32_t>::max();
  using ctr = counter<uint32_t, 2>;
  {
    ctr input{max32, 1};
    ctr expected = (input + max32) + uint32_t{2};
    ctr actual = input + (uint64_t{max32} + uint64_t{2});
    assert_are_equal(actual, expected);
  }
}

void test_subtract_small()
{
  using ctr = counter<uint32_t, 2U>;

  auto test = [](ctr counter, uint32_t subtrahend, ctr expected) {
    auto actual = counter - subtrahend;
    assert_are_equal(actual, expected);
  };

  constexpr uint32_t max32 = std::numeric_limits<uint32_t>::max();
  test({max32, 1U}, 2U, {max32 - 2U, 1U});
  test({1U, max32}, 2U, {max32, max32 - 1U});
  test({3U, max32}, 2U, {1U, max32});
}

void test_subtract_large()
{
  constexpr uint32_t max32 = std::numeric_limits<uint32_t>::max();
  auto test = [](auto ctr, uint32_t a1, uint32_t a2) {
    auto expected = (ctr - a1) - a2;
    auto actual = ctr - (uint64_t{a1} + uint64_t{a2});
    assert_are_equal(actual, expected);
  };

  {
    using ctr3 = counter<uint32_t, 3>;
    test(ctr3{max32, 1, 1}, max32, 2);
  }
}

void test_reinterpret()
{
  constexpr qtfy::random::counter<uint16_t, 4> arr16{0x1234, 0x5678, 0x9012, 0x3456};
  constexpr qtfy::random::counter<uint32_t, 2> arr32{0x56781234, 0x34569012};

  auto expected16 = qtfy::random::reinterpret<uint16_t>(arr32);
  auto expected32 = qtfy::random::reinterpret<uint32_t>(arr16);

  assert_are_equal(expected16, arr16);
  assert_are_equal(expected32, arr32);
}

int main()
{
  test_increment_decrement();
  test_add_small();
  test_add_large();
  test_subtract_small();
  test_subtract_large();
  test_reinterpret();
  std::cout << "success";
}