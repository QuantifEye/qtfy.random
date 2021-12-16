#include "qtfy/random.hpp"
#include "test_tools.hpp"

void test_rotate_left()
{
  uint32_t x = 0x12345678U;
  uint32_t actual = qtfy::random::utilities::rotate_left<4>(x);
  uint32_t expected = 0x23456781U;
  assert_are_equal(actual, expected);
}

void test_big_mul() {}

int main()
{
  test_rotate_left();
  test_big_mul();
  std::cout << "success";
}
