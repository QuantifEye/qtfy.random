#ifndef QUANTIFEYE_TEST_TOOLS_HPP
#define QUANTIFEYE_TEST_TOOLS_HPP

#include <iostream>
#include <random>
#include <stdexcept>
#include "qtfy/coro/generator.hpp"
#include "qtfy/random.hpp"

namespace qtfy::random {

using namespace qtfy::coro;

void assert_are_equal(auto&& left, auto&& right)
{
  if (left != right)
  {
    throw std::exception{};
  }
}

template <class word_t, size_t words>
generator<counter<word_t, words>> counters(size_t amount)
{
  auto ctr = counter<word_t, words>{};
  auto engine = std::mt19937_64{};
  for (size_t i{}; i < amount; ++i)
  {
    for (auto& c : ctr)
    {
      c = static_cast<word_t>(engine());
    }

    co_yield ctr;
  }
}

}  // namespace qtft::random
#endif  // QUANTIFEYE_TEST_TOOLS_HPP
