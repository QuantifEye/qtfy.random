#include <algorithm>
#include <execution>
#include <memory>
#include <random>
#include <ranges>
#include <vector>
#include "qtfy/coro/generator.hpp"
#include "qtfy/random.hpp"
#include "tools.hpp"

using qtfy::coro::generator;

generator<uint64_t> random_engine_stream(uint64_t key)
{
  using namespace qtfy::random;
  threefry4x64<> gen{{key, 0, 0, 0}};
  while (true)
  {
    co_yield gen();
  }
}

generator<double> standard_normal_stream(uint64_t key)
{
  using namespace qtfy::random;
  threefry4x64<> gen{{key, 0, 0, 0}};
  std::normal_distribution<double> dist{};
  while (true)
  {
    co_yield dist(gen);
  }
}

constexpr double calculate_pi(uint64_t iterations) noexcept
{
  const auto divisor = static_cast<double>(iterations);
  auto engine = qtfy::random::threefry4x64<>{};
  uint64_t count{};
  do
  {
    double x = engine.next_canonical();
    double y = engine.next_canonical();
    if (x * x + y * y <= 1.0)
    {
      ++count;
    }
  } while (--iterations != 0);

  return 4.0 * static_cast<double>(count) / divisor;
}

int main()
{
  using namespace std::views;
  using std::ranges::count;
  using std::ranges::count_if;
  using std::ranges::sort;

  auto print_line = [](auto&& x) { std::cout << x << '\n'; };
  auto new_line = []() { std::cout << '\n'; };

  print_line("calculate pi:");
  print_line(calculate_pi(1000));

  new_line();

  print_line("initialise a stream with a key, and print some values.");
  auto random_numbers = random_engine_stream(1);
  for (auto x : random_numbers | take(10))
  {
    print_line(x);
  }

  new_line();

  print_line("count even numbers from stream:");
  auto is_even = [](auto x) { return x % 2U == 0; };
  auto even_count = count_if(random_numbers | take(10000), is_even);
  print_line(even_count);

  new_line();

  print_line("print some normally distributed values:");
  auto normal_values = standard_normal_stream(1);
  for (auto x : normal_values | take(10))
  {
    print_line(x);
  }

  new_line();

  print_line("calculate the normal percentile that is represented by 0.675:");
  auto amount = 100000;
  auto c =
      count_if(normal_values | take(amount), [](auto x) { return x < 0.675; });
  print_line(c / static_cast<double>(amount));
}
