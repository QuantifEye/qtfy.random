#ifndef QUANTIFEYE_TEST_TOOLS_HPP
#define QUANTIFEYE_TEST_TOOLS_HPP

#include <iostream>
#include <stdexcept>

void assert_are_equal(auto&& left, auto&& right)
{
  if (left != right)
  {
    throw std::exception{};
  }
}

template <class TGen, class key_t = typename TGen::key_type, class ctr_t = typename TGen::counter_type>
void test(ctr_t ctr, key_t key, ctr_t expected)
{
  auto actual = TGen::bijection(ctr, TGen::set_key(key));
  assert_are_equal(actual, expected);
}

#endif  // QUANTIFEYE_TEST_TOOLS_HPP
