#include <vector>

#include "qtfy/random.hpp"
#include "test_tools.hpp"

using namespace qtfy::random;

constexpr auto mock_bijection = [](counter<uint32_t, 4> ctr) constexpr noexcept {
  auto c0 = ctr[0] * 4;
  return counter<uint32_t, 4>{c0, c0 + 1U, c0 + 2U, c0 + 3U};
};

constexpr auto make_mock_engine(counter<uint32_t, 4> c = {}) noexcept
{
  return make_counter_based_engine<uint32_t>(c, mock_bijection);
}

void test_mock_bijection()
{
  using ctr_t = counter<uint32_t, 4>;
  {
    ctr_t expected = {0U, 1U, 2U, 3U};
    ctr_t actual = mock_bijection({0U, 0U, 0U, 0U});
    assert_are_equal(expected, actual);
  }
  {
    ctr_t expected = {4U, 5U, 6U, 7U};
    ctr_t actual = mock_bijection({1U, 0U, 0U, 0U});
    assert_are_equal(expected, actual);
  }
  {
    ctr_t expected = {8U, 9U, 10U, 11U};
    ctr_t actual = mock_bijection({2U, 0U, 0U, 0U});
    assert_are_equal(expected, actual);
  }
}

void test_call_operator()
{
  auto gen = make_mock_engine();
  for (uint32_t i{}; i < 1000U; ++i)
  {
    assert_are_equal(gen(), i);
  }
}

void test_discard()
{
  auto test = [](std::vector<uint32_t> expected, unsigned jump_size) {
    std::vector<uint32_t> actual{};
    auto gen = make_mock_engine();
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
  auto gen64 = make_threefry_engine<uint64_t, 4, 20, uint64_t>({}, {});
  auto gen32 = make_threefry_engine<uint64_t, 4, 20, uint32_t>({}, {});

  uint64_t two32 = uint64_t{UINT32_MAX} + uint64_t{1};
  for (int i = 0; i < 100; ++i)
  {
    uint64_t x = gen64();

    uint64_t y0 = gen32();
    uint64_t y1 = gen32();
    uint64_t y = y0 + (y1 * two32);

    assert_are_equal(x, y);
  }
}

int main()
{
  test_mock_bijection();
  test_call_operator();
  test_call_operator();
  test_discard();
  test_different_return_types();
  std::cout << "success";
}