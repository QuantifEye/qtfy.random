#include <random>
#include <vector>
#include "qtfy/coro/generator.hpp"
#include "qtfy/random.hpp"
#include "test_tools.hpp"
using namespace qtfy::random;
using namespace qtfy::coro;

template <class word_t, size_t words>
struct test_case
{
  counter<word_t, words> ctr;
  counter<word_t, words> key;
  counter<word_t, words> expected;
};

template <class word_t, size_t words, unsigned rounds, word_t t0 = 0U,
          word_t t1 = 0U>
void test(std::vector<test_case<word_t, words>> cases)
{
  for (auto x : cases)
  {
    auto bijection = threefry_factory<word_t, words, rounds, t0, t1>(x.key);
    auto actual = bijection(x.ctr);
    if (actual != x.expected)
    {
      throw std::exception{};
    }
  }
}



void test_run_time_key()
{
  test<uint64_t, 4, 13>({{{0x243f6a8885a308d3, 0x13198a2e03707344,
                           0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0x452821e638d01377, 0xbe5466cf34e90c6c,
                           0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917},
                          {0x4361288ef9c1900c, 0x8717291521782833,
                           0x0d19db18c20cf47e, 0xa0b41d63ac8581e5}},
                         {{},
                          {},
                          {0x4071fabee1dc8e05, 0x02ed3113695c9c62,
                           0x397311b5b89f9d49, 0xe21292c3258024bc}},
                         {{UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {0x7eaed935479722b5, 0x90994358c429f31c,
                           0x496381083e07a75b, 0x627ed0d746821121}}});

  test<uint64_t, 4, 20>({{{0x243f6a8885a308d3, 0x13198a2e03707344,
                           0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0x452821e638d01377, 0xbe5466cf34e90c6c,
                           0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd},
                          {0xa7e8fde591651bd9, 0xbaafd0c30138319b,
                           0x84a5c1a729e685b9, 0x901d406ccebc1ba4}},
                         {{},
                          {},
                          {0x09218ebde6c85537, 0x55941f5266d86105,
                           0x4bd25e16282434dc, 0xee29ec846bd2e40b}},
                         {{UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {0x29c24097942bba1b, 0x0371bbfb0f6f4e11,
                           0x3c231ffa33f83a1c, 0xcd29113fde32d168}}});

  test<uint64_t, 4, 72>({{{0x243f6a8885a308d3, 0x13198a2e03707344,
                           0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0x452821e638d01377, 0xbe5466cf34e90c6c,
                           0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd},
                          {0xacf412ccaa3b2270, 0xc9e99bd53f2e9173,
                           0x43dad469dc825948, 0xfbb19d06c8a2b4dc}},
                         {{},
                          {},
                          {0x94eeea8b1f2ada84, 0xadf103313eae6670,
                           0x952419a1f4b16d53, 0xd83f13e63c9f6b11}},
                         {{UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {0x11518c034bc1ff4c, 0x193f10b8bcdcc9f7,
                           0xd024229cb58f20d8, 0x563ed6e48e05183f}}});

  test<uint64_t, 2, 13>({{{0x243f6a8885a308d3, 0x13198a2e03707344},
                          {0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0xc3aac71561042993, 0x3fe7ae8801aff316}},
                         {{}, {}, {0xf167b032c3b480bd, 0xe91f9fee4b7a6fb5}},
                         {{UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX},
                          {0xccdec5c917a874b1, 0x4df53abca26ceb01}}});

  test<uint64_t, 2, 20>({{{0x243f6a8885a308d3, 0x13198a2e03707344},
                          {0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0x263c7d30bb0f0af1, 0x56be8361d3311526}},
                         {{}, {}, {0xc2b6e3a8c2c69865, 0x6f81ed42f350084d}},
                         {{UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX},
                          {0xe02cb7c4d95d277a, 0xd06633d0893b8b68}}});

  test<uint64_t, 2, 32>({{{0x243f6a8885a308d3, 0x13198a2e03707344},
                          {0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0xdad492f32efbd0c4, 0xb6d7d0cd1f193e84}},
                         {{}, {}, {0x38ba854d7f13cfb3, 0xd02fca729d54fadc}},
                         {{UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX},
                          {0x6b532f4f6e288646, 0x0388f1ec135ee18e}}});

  test<uint32_t, 4, 13>(
      {{{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344},
        {0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89},
        {0x4aa71d8f, 0x734738c2, 0x431fc6a8, 0xae6debf1}},
       {{}, {}, {0x531c7e4f, 0x39491ee5, 0x2c855a92, 0x3d6abf9a}},
       {{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {0xc4189358, 0x1c9cc83a, 0xd5881c67, 0x6a0a89e0}}});

  test<uint32_t, 4, 20>(
      {{{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344},
        {0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89},
        {0x59cd1dbb, 0xb8879579, 0x86b5d00c, 0xac8b6d84}},
       {{}, {}, {0x9c6ca96a, 0xe17eae66, 0xfc10ecd4, 0x5256a7d8}},
       {{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {0x2a881696, 0x57012287, 0xf6c7446e, 0xa16a6732}}});

  test<uint32_t, 4, 72>(
      {{{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344},
        {0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89},
        {0x09930adf, 0x7f27bd55, 0x9ed68ce1, 0x97f803f6}},
       {{}, {}, {0x93171da6, 0x9220326d, 0xb392b7b1, 0xff58a002}},
       {{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {0x60743f3d, 0x9961e684, 0xaab21c34, 0x8c65fb7d}}});

  test<uint32_t, 2, 13>({{{0x243f6a88, 0x85a308d3},
                          {0x13198a2e, 0x03707344},
                          {0xba3e4725, 0xf27d669e}},
                         {{}, {}, {0x9d1c5ec6, 0x8bd50731}},
                         {{UINT32_MAX, UINT32_MAX},
                          {UINT32_MAX, UINT32_MAX},
                          {0xfd36d048, 0x2d17272c}}});

  test<uint32_t, 2, 20>({{{0x243f6a88, 0x85a308d3},
                          {0x13198a2e, 0x03707344},
                          {0xc4923a9c, 0x483df7a0}},
                         {{}, {}, {0x6b200159, 0x99ba4efe}},
                         {{UINT32_MAX, UINT32_MAX},
                          {UINT32_MAX, UINT32_MAX},
                          {0x1cb996fc, 0xbb002be7}}});

  test<uint32_t, 2, 32>({{{0x243f6a88, 0x85a308d3},
                          {0x13198a2e, 0x03707344},
                          {0xe2827716, 0xc3c05cdf}},
                         {{}, {}, {0xcee3d47e, 0xa23dfd5c}},
                         {{UINT32_MAX, UINT32_MAX},
                          {UINT32_MAX, UINT32_MAX},
                          {0x6e2fe0d0, 0xb1b76f82}}});
}

template <class word_t, size_t words, unsigned rounds, word_t t0, word_t t1,
          counter<word_t, words> key>
void test()
{
  auto bijection1 = threefry_factory<word_t, words, rounds, t0, t1>(key);
  auto bijection2 = threefry_factory<word_t, words, rounds, t0, t1, key>();
  for (auto ctr : counters<word_t, words>(100))
  {
    auto result1 = bijection1(ctr);
    auto result2 = bijection2(ctr);
    if (result1 != result2)
    {
      throw std::exception{};
    }
  }
}



void compare_runtime_and_compile_time_key()
{
  test<uint64_t, 4, 72, 0, 0, {}>();
  test<uint32_t, 2, 72, 0, 0, {}>();
  test<uint64_t, 4, 72, 0, 0, {}>();
  test<uint32_t, 2, 72, 0, 0, {}>();

  test<uint64_t, 4, 20, 0, 0, {1, 2, 3, 4}>();
  test<uint32_t, 2, 20, 0, 0, {5, 6}>();
  test<uint64_t, 4, 20, 0, 0, {7, 8, 9, 1}>();
  test<uint32_t, 2, 20, 0, 0, {2, 3}>();

  test<uint64_t, 4, 72, 2, 3, {}>();
  test<uint32_t, 2, 72, 4, 5, {}>();
  test<uint64_t, 4, 72, 6, 7, {}>();
  test<uint32_t, 2, 72, 8, 9, {}>();

  test<uint64_t, 4, 20, 1, 2, {1, 2, 3, 4}>();
  test<uint32_t, 2, 20, 3, 4, {5, 6}>();
  test<uint64_t, 4, 20, 5, 6, {7, 8, 9, 1}>();
  test<uint32_t, 2, 20, 7, 8, {2, 3}>();
}

void bijection_is_constexpr()
{
  constexpr auto bij = threefry_factory<uint64_t, 4, 72, 0, 0>({});
  constexpr auto x = bij({});
  using t = std::array<int, x[0]>;
}

int main()
{
  test_run_time_key();
  compare_runtime_and_compile_time_key();
  bijection_is_constexpr();
  std::cout << "success" << '\n';
}