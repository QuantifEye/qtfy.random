#include "qtfy/random.hpp"
#include "tools.hpp"

int main()
{
  using namespace qtfy::random;
  using ctr_t = counter<uint32_t, 4>;

  // counters support incrementing
  {
    ctr_t ctr{1, 4, 7, 9};
    ++ctr;
    assert_equal(ctr, ctr_t{2, 4, 7, 9});
  }

  // counters support decrementing
  {
    ctr_t ctr{2, 4, 7, 9};
    --ctr;
    assert_equal(ctr, ctr_t{1, 4, 7, 9});
  }

  // note from the last two examples that the least significant bits of counter
  // are at the smallest index.

  // counters support addition for unsigned integrals
  {
    ctr_t ctr{1, 2, 3, 4};
    assert_equal(ctr + 5U, ctr_t{6, 2, 3, 4});
  }

  // counters support addition in place for unsigned integrals
  {
    ctr_t ctr{1, 2, 3, 4};
    ctr += 5U;
    assert_equal(ctr, ctr_t{6, 2, 3, 4});
  }

  // counters support subtraction for unsigned integrals
  {
    ctr_t ctr{10, 2, 3, 4};
    assert_equal(ctr - 5U, ctr_t{5, 2, 3, 4});
  }

  // counters support subtraction in place for unsigned integrals
  {
    ctr_t ctr{10, 2, 3, 4};
    ctr -= 5U;
    assert_equal(ctr, ctr_t{5, 2, 3, 4});
  }

  // can subtract and add numbers larger than what individual words can manage.
  {
    constexpr auto two_to_the_32 = uint64_t{1} << 32U;
    assert_equal(ctr_t{10, 2, 3, 4} - two_to_the_32, ctr_t{10, 1, 3, 4});
  }
}
