#include <execution>
#include <memory>
#include <random>
#include <ranges>
#include <type_traits>
#include <vector>

#include "qtfy/coro/generator.hpp"
#include "qtfy/random.hpp"
#include "tools.hpp"

using namespace qtfy::random;

constexpr double calculate_pi(const uint64_t iterations) noexcept
{
  auto engine = qtfy::random::make_threefry_engine<uint64_t, 4, 20>({}, {});
  uint64_t count{};
  for (size_t i{}; i < iterations; ++i)
  {
    double x = engine.next_canonical();
    double y = engine.next_canonical();
    if (x * x + y * y <= 1.0)
    {
      ++count;
    }
  };

  return 4.0 * static_cast<double>(count) / static_cast<double>(iterations);
}

void write(auto && v)
{
  std::cout << v << '\n';
}

qtfy::coro::generator<uint64_t> with_runtime_key(const int buffer_count)
{
  // the procedural logic for the generators are implemented in trait classes that
  // provide static function.

  // the first two template arguments describe the counter that will be used. that is counter_t below.

  // the third argument is the number of rounds of "encryption" that needs to happen each
  // time a random buffer is generated.
  // a greater number might result in higher quality random numbers, but will be slower.
  using trait_t = threefry_trait<uint64_t, 4, 7>;

  // we parameterize a cprng (counter based random number generator)
  // with a key, the type of which is provided as a using declaration
  // in the trait class.
  using user_provided_key_t = typename trait_t::key_type;
  static_assert(std::same_as<user_provided_key_t, counter<uint64_t, 4>>);

  // depending on the trait, the key used by the cprng may not be the
  // same as the user provided key
  // to get this internal key we use the method trait_t::set_key(user_provided_key_t k)
  // the return type of which can also be accessed with the using declaration
  using internal_key_t = typename trait_t::internal_key_type;
  static_assert(std::same_as<internal_key_t, counter<uint64_t, 5>>);

  const internal_key_t internal_key = trait_t::set_key({1, 2, 3, 4});

  // trait_t also has a typedef for a counter_t which is used to
  // advance the state of the cprng
  using counter_t = typename trait_t::counter_type;
  static_assert(std::same_as<counter_t, counter<uint64_t, 4>>);

  // the function called bijection is the one that does most of the work.
  // bijection takes a counter_t as a first argument and an internal_key_t as the second argument.
  // bijection returns a counter_t
  counter_t ctr{1, 2, 3, 4};
  for (int i = 0; i < buffer_count; ++i)
  {
    // calling bijection creates a new buffer of random numbers.
    counter_t buffer = trait_t::bijection(ctr, internal_key);
    for (auto item : buffer)
    {
      co_yield item;
    }

    // after returning all values from the buffer we increment
    // the counter and go to start of loop to create another buffer.
    ++ctr;
  }
}

qtfy::coro::generator<uint64_t> with_compile_time_key(const int buffer_count)
{
  // the class containing the implementation
  using trait_t = threefry_trait<uint64_t, 4, 7>;

  // the type of the first argument and return type of bijection,
  using counter_t = typename trait_t::counter_type;

  // the key type used by bijection
  using internal_key_t = typename trait_t::internal_key_type;

  // because we know the key at compile time, and the
  // code is constexpr friendly, we can determine the internal
  // key value at compile time.
  constexpr internal_key_t internal_key = trait_t::set_key({1, 2, 3, 4});

  counter_t ctr{1, 2, 3, 4};
  for (int i = 0; i < buffer_count; ++i)
  {
    // as in the previous example, we call bijection to create a new buffer
    // but this time we pass in the internal key as a template parameter.
    counter_t buffer = trait_t::template bijection<internal_key>(ctr);
    for (auto number : buffer)
    {
      co_yield number;
    }

    ++ctr;
  }
}

qtfy::coro::generator<uint64_t> unifying_runtime_and_compile_time_key_interface()
{
  // unfortunately the version of bijection that take a runtime key, and the version that takes a compile-time key
  // have a different number of non-template arguments, so this makes it hard to use interchangeably

  // luckily the trait also provides a way for us to bind the key to the function.
  using trait_t = threefry_trait<uint64_t, 4, 7>;
  using counter_t = typename trait_t::counter_type;

  // to create a bijection with a runtime key, we pass it in the regular way
  // note that we do not have to use set_key anymore because make_bijection does
  // this for us.

  auto runtime_key_lambda = trait_t::make_bijection({1, 2, 3, 4});

  // to create a lambda with a compile-time key, we use the overloaded function
  // and provide a key as a template argument.
  auto compile_time_key_lambda = trait_t::template make_bijection<{1, 2, 3, 4}>();

  // in each case the returned item is a lambda that is takes a counter_t, and returns a counter_t
  // and provide exactly the same numeric results.

  // note that both runtime_key_lambda and compile_time_key_lambda are constexpr able, and if the key is
  // known at compile time either could be made constexpr.

  const counter_t ctr{1, 2, 3, 4};
  for (auto number : runtime_key_lambda(ctr))
  {
    co_yield number;
  }

  for (auto number : compile_time_key_lambda(ctr))
  {
    co_yield number;
  }

  // since the syntax used to create compile_time_lambda is tricky for some,
  // we also provide free functions to achieve the same.
  // here is the example for threefry

  // key passed as parameter.
  auto runtime_key_lambda2 = make_threefry_functor<uint64_t, 4, 7>({1, 2, 3, 4});

  // key passed as template parameter.
  auto compile_time_key_lambda2 = make_threefry_functor<uint64_t, 4, 7, {1, 2, 3, 4}>();
}


int main() {

  for (auto x : with_runtime_key(10))
  {
    write(x);
  }

  for (auto x : with_compile_time_key(10))
  {
    write(x);
  }

  for (auto x : unifying_runtime_and_compile_time_key_interface())
  {
    write(x);
  }
}
