#include <iostream>
#include <vector>
#include "qtfy/random.hpp"
#include "test_tools.hpp"

using namespace qtfy::random;

template <class word_t, size_t words>
struct test_case
{
  counter<word_t, words> ctr;
  counter<word_t, words / 2> key;
  counter<word_t, words> expected;
};

template <class word_t, size_t words, unsigned rounds>
auto test(std::vector<test_case<word_t, words>> cases)
{
  for (auto x : cases)
  {
    auto bijection = philox_factory<word_t, words, rounds>(x.key);
    auto actual = bijection(x.ctr);
    if (actual != x.expected)
    {
      throw std::exception{};
    }
  }
}

auto test_runtime_key()
{
  test<uint64_t, 4, 7>({{{0x243f6a8885a308d3, 0x13198a2e03707344,
                          0xa4093822299f31d0, 0x082efa98ec4e6c89},
                         {0x452821e638d01377, 0xbe5466cf34e90c6c},
                         {0x513a366704edf755, 0xf05d9924c07044d3,
                          0xbef2cb9cbea74c6c, 0x8db948de4caa1f8a}},
                        {{},
                         {},
                         {0x5dc8ee6268ec62cd, 0x139bc570b6c125a0,
                          0x84d6deb4fb65f49e, 0xaff7583376d378c2}},
                        {{UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                         {UINT64_MAX, UINT64_MAX},
                         {0x071dd84367903154, 0x48e2bbdc722b37d1,
                          0x6afa9890bb89f76c, 0x9194c8d8ada56ac7}}});

  test<uint64_t, 4, 10>({{{0x243f6a8885a308d3, 0x13198a2e03707344,
                           0xa4093822299f31d0, 0x082efa98ec4e6c89},
                          {0x452821e638d01377, 0xbe5466cf34e90c6c},
                          {0xa528f45403e61d95, 0x38c72dbd566e9788,
                           0xa5a1610e72fd18b5, 0x57bd43b5e52b7fe6}},
                         {{},
                          {},
                          {0x16554d9eca36314c, 0xdb20fe9d672d0fdc,
                           0xd7e772cee186176b, 0x7e68b68aec7ba23b}},
                         {{UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX, UINT64_MAX},
                          {0x87b092c3013fe90b, 0x438c3c67be8d0224,
                           0x9cc7d7c69cd777b6, 0xa09caebf594f0ba0}}});

  test<uint64_t, 2, 7>({{{0x243f6a8885a308d3, 0x13198a2e03707344},
                         {0xa4093822299f31d0},
                         {0x98ed1534392bf372, 0x67528b1568882fd5}},
                        {{}, {}, {0xb41da69fbfefc666, 0x511e9ce1a5534056}},
                        {{UINT64_MAX, UINT64_MAX},
                         {UINT64_MAX},
                         {0xa4696cc04462015d, 0x724782dae17169e9}}});

  test<uint64_t, 2, 10>({{{0x243f6a8885a308d3, 0x13198a2e03707344},
                          {0xa4093822299f31d0},
                          {0x0a5e742c2997341c, 0xb0f883d38000de5d}},
                         {{}, {}, {0xca00a0459843d731, 0x66c24222c9a845b5}},
                         {{UINT64_MAX, UINT64_MAX},
                          {UINT64_MAX},
                          {0x65b021d60cd8310f, 0x4d02f3222f86df20}}});

  test<uint32_t, 4, 7>(
      {{{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344},
        {0xa4093822, 0x299f31d0},
        {0x4dfccaba, 0x190a87f0, 0xc47362ba, 0xb6b5242a}},
       {{}, {}, {0x5f6fb709, 0x0d893f64, 0x4f121f81, 0x4f730a48}},
       {{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {UINT32_MAX, UINT32_MAX},
        {0x5207ddc2, 0x45165e59, 0x4d8ee751, 0x8c52f662}}});

  test<uint32_t, 4, 10>(
      {{{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344},
        {0xa4093822, 0x299f31d0},
        {0xd16cfe09, 0x94fdcceb, 0x5001e420, 0x24126ea1}},
       {{}, {}, {0x6627e8d5, 0xe169c58d, 0xbc57ac4c, 0x9b00dbd8}},
       {{UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX},
        {UINT32_MAX, UINT32_MAX},
        {0x408f276d, 0x41c83b0e, 0xa20bc7c6, 0x6d5451fd}}});

  test<uint32_t, 2, 7>(
      {{{0x243f6a88, 0x85a308d3}, {0x13198a2e}, {0xbedbbe6b, 0xe4c770b3}},
       {{}, {}, {0x257a3673, 0xcd26be2a}},
       {{UINT32_MAX, UINT32_MAX}, {UINT32_MAX}, {0xab302c4d, 0x3dc9d239}}});

  test<uint32_t, 2, 10>(
      {{{}, {}, {0xff1dae59, 0x6cd10df2}},
       {{UINT32_MAX, UINT32_MAX}, {UINT32_MAX}, {0x2c3f628b, 0xab4fd7ad}},
       {{0x243f6a88, 0x85a308d3}, {0x13198a2e}, {0xdd7ce038, 0xf62a4c12}}});
}

template <class word_t, size_t words, unsigned rounds,
          counter<word_t, words / 2> key>
void test()
{
  auto bijection1 = philox_factory<word_t, words, rounds>(key);
  auto bijection2 = philox_factory<word_t, words, rounds, key>();
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
  test<uint64_t, 4, 16, {}>();
  test<uint32_t, 2, 16, {}>();
  test<uint64_t, 4, 16, {}>();
  test<uint32_t, 2, 16, {}>();

  test<uint64_t, 4, 7, {1, 2}>();
  test<uint32_t, 2, 7, {5}>();
  test<uint64_t, 4, 7, {7, 8}>();
  test<uint32_t, 2, 7, {2}>();

  test<uint64_t, 4, 16, {}>();
  test<uint32_t, 2, 16, {}>();
  test<uint64_t, 4, 16, {}>();
  test<uint32_t, 2, 16, {}>();

  test<uint64_t, 4, 7, {1, 2}>();
  test<uint32_t, 2, 7, {5}>();
  test<uint64_t, 4, 7, {7, 8}>();
  test<uint32_t, 2, 7, {2}>();
}

void bijection_is_constexpr()
{
  constexpr auto bij = philox_factory<uint64_t, 4, 16>({});
  constexpr auto x = bij({});
  using t = std::array<int, x[0]>;
}

int main()
{
  test_runtime_key();
  compare_runtime_and_compile_time_key();
  bijection_is_constexpr();
  std::cout << "success";
}
