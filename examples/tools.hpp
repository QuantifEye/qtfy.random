#ifndef QUANTIFEYE_TOOLS_HPP
#define QUANTIFEYE_TOOLS_HPP
#include <exception>
#include <iostream>

auto assert_equal(auto&& expected, auto&& actual)
{
  if (expected != actual)
  {
    throw ::std::exception{};
  }
}

#endif  // QUANTIFEYE_TOOLS_HPP
