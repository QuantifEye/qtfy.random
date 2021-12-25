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

#endif  // QUANTIFEYE_TEST_TOOLS_HPP
