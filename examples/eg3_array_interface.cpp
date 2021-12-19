#include "qtfy/random.hpp"
#include "tools.hpp"

int main()
{
  using namespace qtfy::random;
  using ctr_t = counter<uint32_t, 4U>;

  // counter and std::array share an interface.
  // this allows the individual parts that make up the counter to be accessed and
  // iterated as though it was an array.

  {
    ctr_t ctr{1U, 2U, 3U, 4U};
    for (auto& c : ctr)
    {
      c += 1U;
    }
    assert_equal(ctr, ctr_t{2U, 3U, 4U, 5U});
  }

  {
    ctr_t ctr{1U, 2U, 3U, 4U};
    for (auto iter = ctr.begin(), end = ctr.end(); iter != end; ++iter)
    {
      *iter += 1U;
    }
    assert_equal(ctr, ctr_t{2U, 3U, 4U, 5U});
  }

  {
    ctr_t ctr{1U, 2U, 3U, 4U};
    assert_equal(ctr.front(), 1U);
    assert_equal(ctr.back(), 4U);
  }

  {
    ctr_t ctr{1U, 2U, 3U, 4U};
    ctr.back() += 3U;
    assert_equal(ctr, ctr_t{1U, 2U, 3U, 7U});
  }
}
