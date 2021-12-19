#include <algorithm>
#include <memory>
#include <vector>
#include <execution>
#include "qtfy/random.hpp"
#include "tools.hpp"

void print(auto&& item) { std::cout << item << '\n'; }

int main()
{
  using namespace qtfy::random;

  // the library provides various counter based engines.
  // all are provided as using declarations in the header random.hpp
  // all counter based can be constructed with a key and a counter.
  // both arguments are of type counter<,>

  // we add the empty parens because we want to accept the default arguments
  using gen_t = threefry4x64<>;

  // how to create a counter based generator
  {
    // we default construct the generator
    // this will set both the counter and the key to zero
    gen_t gen{};
    for (int i = 10; --i;)
    {
      print(gen());
    }
  }

  // in order to create independent random number generators
  // so that we can use each generator independently we
  // have to provide the generators with different keys
  // because keys are of type counter<,> we can simply do this
  // by using the increment operator
  {
    using key_type = gen_t::key_type;
    key_type key{};  // key initialised to zero
    std::vector<gen_t> engines{};
    for (int i{}; i < 100; ++i)
    {
      engines.emplace_back(key);
      ++key;
    }
  }
}
