#include <vector>

#include "qtfy/random.hpp"
#include "test_tools.hpp"

using namespace qtfy::random;

struct mock_trait
{
  using word_type = uint32_t;
  using counter_type = counter<uint32_t, 4>;
  using key_type = counter<uint32_t, 4>;
  using internal_key_type = counter<uint32_t, 4>;

  static internal_key_type set_key(key_type k) noexcept { return k; }

  static counter_type bijection(counter_type ctr, [[maybe_unused]] internal_key_type key) noexcept
  {
    auto c0 = ctr[0] * 4;
    return {c0, c0 + 1U, c0 + 2U, c0 + 3U};
  }
};

void test_mock_trait()
{
  using ctr_t = counter<uint32_t, 4>;
  {
    ctr_t expected = {0U, 1U, 2U, 3U};
    ctr_t actual = mock_trait::bijection({0U, 0U, 0U, 0U}, {});
    assert_are_equal(expected, actual);
  }
  {
    ctr_t expected = {4U, 5U, 6U, 7U};
    ctr_t actual = mock_trait::bijection({1U, 0U, 0U, 0U}, {});
    assert_are_equal(expected, actual);
  }
  {
    ctr_t expected = {8U, 9U, 10U, 11U};
    ctr_t actual = mock_trait::bijection({2U, 0U, 0U, 0U}, {});
    assert_are_equal(expected, actual);
  }
}

void test_call_operator()
{
  counter_based_engine<mock_trait> gen{};
  for (uint32_t i{}; i < 1000U; ++i)
  {
    assert_are_equal(gen(), i);
  }
}

void test_discard()
{
  auto test = [](std::vector<uint32_t> expected, unsigned jump_size) {
    std::vector<uint32_t> actual{};
    counter_based_engine<mock_trait> gen{};
    while (actual.size() < expected.size())
    {
      actual.push_back(gen());
      gen.discard(jump_size);
    }
    assert_are_equal(expected, actual);
  };

  test({0, 2, 4, 6, 8, 10}, 1);
  test({0, 8, 16, 24, 32, 40}, 7);
  test({0, 20, 40, 60, 80}, 19);
}

void test_different_return_types()
{
  qtfy::random::threefry_engine<uint32_t, 4, 20, uint64_t> gen1{};
  qtfy::random::threefry_engine<uint32_t, 4, 20, uint32_t> gen2{};

  uint64_t two32 = uint64_t{UINT32_MAX} + uint64_t{1};
  for (int i = 0; i < 100; ++i)
  {
    uint64_t x = gen1();

    uint64_t y0 = gen2();
    uint64_t y1 = gen2();
    uint64_t y = y0 + (y1 * two32);

    assert_are_equal(x, y);
  }
}

int main()
{
  test_mock_trait();
  test_call_operator();
  test_call_operator();
  test_discard();
  test_different_return_types();
  std::cout << "success";
}