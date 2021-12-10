#include <exception>
#include <iostream>
#include <vector>
#include "random.h"

using namespace qtfy::random;

auto assert_equal(auto l, auto r)
{
  if (l != r)
  {
    throw std::exception{};
  }
}

template <class TGen>
static auto test(auto ctr, auto key, auto expected)
{
  auto actual = TGen::bijection(ctr, TGen::set_key(key));
  assert_equal(actual, expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_threefry(Counter<TWord, words> ctr, Counter<TWord, words> key, Counter<TWord, words> expected)
{
  test<ThreeFry<TWord, words, rounds>>(ctr, key, expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_threefry_min(Counter<TWord, words> expected)
{
  test<ThreeFry<TWord, words, rounds>>(Counter<TWord, words>::min(), Counter<TWord, words>::min(), expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_threefry_max(Counter<TWord, words> expected)
{
  test<ThreeFry<TWord, words, rounds>>(Counter<TWord, words>::max(), Counter<TWord, words>::max(), expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_philox(Counter<TWord, words> ctr, Counter<TWord, words / 2U> key, Counter<TWord, words> expected)
{
  test<Philox<TWord, words, rounds>>(ctr, key, expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_philox_min(Counter<TWord, words> expected)
{
  test<Philox<TWord, words, rounds>>(Counter<TWord, words>::min(), Counter<TWord, words / 2U>::min(), expected);
}

template <class TWord, unsigned words, unsigned rounds>
auto test_philox_max(Counter<TWord, words> expected)
{
  test<Philox<TWord, words, rounds>>(Counter<TWord, words>::max(), Counter<TWord, words / 2U>::max(), expected);
}

auto TestThreeFry64x4()
{
  test_threefry<uint64_t, 4, 13>({0x243f6a8885a308d3, 0x13198a2e03707344, 0xa4093822299f31d0, 0x082efa98ec4e6c89},
                                 {0x452821e638d01377, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd, 0x3f84d5b5b5470917},
                                 {0x4361288ef9c1900c, 0x8717291521782833, 0x0d19db18c20cf47e, 0xa0b41d63ac8581e5});
  test_threefry_min<uint64_t, 4, 13>({0x4071fabee1dc8e05, 0x02ed3113695c9c62, 0x397311b5b89f9d49, 0xe21292c3258024bc});
  test_threefry_max<uint64_t, 4, 13>({0x7eaed935479722b5, 0x90994358c429f31c, 0x496381083e07a75b, 0x627ed0d746821121});
  test_threefry<uint64_t, 4, 20>({0x243f6a8885a308d3, 0x13198a2e03707344, 0xa4093822299f31d0, 0x082efa98ec4e6c89},
                                 {0x452821e638d01377, 0xbe5466cf34e90c6c, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd},
                                 {0xa7e8fde591651bd9, 0xbaafd0c30138319b, 0x84a5c1a729e685b9, 0x901d406ccebc1ba4});
  test_threefry_min<uint64_t, 4, 20>({0x09218ebde6c85537, 0x55941f5266d86105, 0x4bd25e16282434dc, 0xee29ec846bd2e40b});
  test_threefry_max<uint64_t, 4, 20>({0x29c24097942bba1b, 0x0371bbfb0f6f4e11, 0x3c231ffa33f83a1c, 0xcd29113fde32d168});
  test_threefry<uint64_t, 4, 72>({0x243f6a8885a308d3, 0x13198a2e03707344, 0xa4093822299f31d0, 0x082efa98ec4e6c89},
                                 {0x452821e638d01377, 0xbe5466cf34e90c6c, 0xbe5466cf34e90c6c, 0xc0ac29b7c97c50dd},
                                 {0xacf412ccaa3b2270, 0xc9e99bd53f2e9173, 0x43dad469dc825948, 0xfbb19d06c8a2b4dc});
  test_threefry_min<uint64_t, 4, 72>({0x94eeea8b1f2ada84, 0xadf103313eae6670, 0x952419a1f4b16d53, 0xd83f13e63c9f6b11});
  test_threefry_max<uint64_t, 4, 72>({0x11518c034bc1ff4c, 0x193f10b8bcdcc9f7, 0xd024229cb58f20d8, 0x563ed6e48e05183f});
}

auto TestThreeFry32x4()
{
  Counter<uint32_t, 4> ctr{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344};
  Counter<uint32_t, 4> key{0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89};
  test_threefry<uint32_t, 4, 13>(ctr, key, {0x4aa71d8f, 0x734738c2, 0x431fc6a8, 0xae6debf1});
  test_threefry_min<uint32_t, 4, 13>({0x531c7e4f, 0x39491ee5, 0x2c855a92, 0x3d6abf9a});
  test_threefry_max<uint32_t, 4, 13>({0xc4189358, 0x1c9cc83a, 0xd5881c67, 0x6a0a89e0});
  test_threefry<uint32_t, 4, 20>(ctr, key, {0x59cd1dbb, 0xb8879579, 0x86b5d00c, 0xac8b6d84});
  test_threefry_min<uint32_t, 4, 20>({0x9c6ca96a, 0xe17eae66, 0xfc10ecd4, 0x5256a7d8});
  test_threefry_max<uint32_t, 4, 20>({0x2a881696, 0x57012287, 0xf6c7446e, 0xa16a6732});
  test_threefry<uint32_t, 4, 72>(ctr, key, {0x09930adf, 0x7f27bd55, 0x9ed68ce1, 0x97f803f6});
  test_threefry_min<uint32_t, 4, 72>({0x93171da6, 0x9220326d, 0xb392b7b1, 0xff58a002});
  test_threefry_max<uint32_t, 4, 72>({0x60743f3d, 0x9961e684, 0xaab21c34, 0x8c65fb7d});
}

auto TestThreeFry64x2()
{
  Counter<uint64_t, 2> ctr{0x243f6a8885a308d3, 0x13198a2e03707344};
  Counter<uint64_t, 2> key{0xa4093822299f31d0, 0x082efa98ec4e6c89};
  test_threefry<uint64_t, 2, 13>(ctr, key, {0xc3aac71561042993, 0x3fe7ae8801aff316});
  test_threefry_min<uint64_t, 2, 13>({0xf167b032c3b480bd, 0xe91f9fee4b7a6fb5});
  test_threefry_max<uint64_t, 2, 13>({0xccdec5c917a874b1, 0x4df53abca26ceb01});
  test_threefry<uint64_t, 2, 20>(ctr, key, {0x263c7d30bb0f0af1, 0x56be8361d3311526});
  test_threefry_min<uint64_t, 2, 20>({0xc2b6e3a8c2c69865, 0x6f81ed42f350084d});
  test_threefry_max<uint64_t, 2, 20>({0xe02cb7c4d95d277a, 0xd06633d0893b8b68});
  test_threefry<uint64_t, 2, 32>(ctr, key, {0xdad492f32efbd0c4, 0xb6d7d0cd1f193e84});
  test_threefry_min<uint64_t, 2, 32>({0x38ba854d7f13cfb3, 0xd02fca729d54fadc});
  test_threefry_max<uint64_t, 2, 32>({0x6b532f4f6e288646, 0x0388f1ec135ee18e});
}

auto TestThreeFry32x2()
{
  Counter<uint32_t, 2> ctr{0x243f6a88, 0x85a308d3};
  Counter<uint32_t, 2> key{0x13198a2e, 0x03707344};
  test_threefry<uint32_t, 2, 13>(ctr, key, {0xba3e4725, 0xf27d669e});
  test_threefry_min<uint32_t, 2, 13>({0x9d1c5ec6, 0x8bd50731});
  test_threefry_max<uint32_t, 2, 13>({0xfd36d048, 0x2d17272c});
  test_threefry<uint32_t, 2, 20>(ctr, key, {0xc4923a9c, 0x483df7a0});
  test_threefry_min<uint32_t, 2, 20>({0x6b200159, 0x99ba4efe});
  test_threefry_max<uint32_t, 2, 20>({0x1cb996fc, 0xbb002be7});
  test_threefry<uint32_t, 2, 32>(ctr, key, {0xe2827716, 0xc3c05cdf});
  test_threefry_min<uint32_t, 2, 32>({0xcee3d47e, 0xa23dfd5c});
  test_threefry_max<uint32_t, 2, 32>({0x6e2fe0d0, 0xb1b76f82});
}

auto TestPhilox64x4()
{
  Counter<uint64_t, 4> ctr{0x243f6a8885a308d3, 0x13198a2e03707344, 0xa4093822299f31d0, 0x082efa98ec4e6c89};
  Counter<uint64_t, 2> key{0x452821e638d01377, 0xbe5466cf34e90c6c};
  test_philox<uint64_t, 4, 7>(ctr, key,
                              {0x513a366704edf755, 0xf05d9924c07044d3, 0xbef2cb9cbea74c6c, 0x8db948de4caa1f8a});
  test_philox_min<uint64_t, 4, 7>({0x5dc8ee6268ec62cd, 0x139bc570b6c125a0, 0x84d6deb4fb65f49e, 0xaff7583376d378c2});
  test_philox_max<uint64_t, 4, 7>({0x071dd84367903154, 0x48e2bbdc722b37d1, 0x6afa9890bb89f76c, 0x9194c8d8ada56ac7});
  test_philox<uint64_t, 4, 10>(ctr, key,
                               {0xa528f45403e61d95, 0x38c72dbd566e9788, 0xa5a1610e72fd18b5, 0x57bd43b5e52b7fe6});
  test_philox_min<uint64_t, 4, 10>({0x16554d9eca36314c, 0xdb20fe9d672d0fdc, 0xd7e772cee186176b, 0x7e68b68aec7ba23b});
  test_philox_max<uint64_t, 4, 10>({0x87b092c3013fe90b, 0x438c3c67be8d0224, 0x9cc7d7c69cd777b6, 0xa09caebf594f0ba0});
}

auto TestPhilox32x4()
{
  Counter<uint32_t, 4> ctr{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344};
  Counter<uint32_t, 2> key{0xa4093822, 0x299f31d0};
  test_philox<uint32_t, 4, 7>(ctr, key, {0x4dfccaba, 0x190a87f0, 0xc47362ba, 0xb6b5242a});
  test_philox_min<uint32_t, 4, 7>({0x5f6fb709, 0x0d893f64, 0x4f121f81, 0x4f730a48});
  test_philox_max<uint32_t, 4, 7>({0x5207ddc2, 0x45165e59, 0x4d8ee751, 0x8c52f662});
  test_philox<uint32_t, 4, 10>(ctr, key, {0xd16cfe09, 0x94fdcceb, 0x5001e420, 0x24126ea1});
  test_philox_min<uint32_t, 4, 10>({0x6627e8d5, 0xe169c58d, 0xbc57ac4c, 0x9b00dbd8});
  test_philox_max<uint32_t, 4, 10>({0x408f276d, 0x41c83b0e, 0xa20bc7c6, 0x6d5451fd});
}

auto TestPhilox64x2()
{
  Counter<uint64_t, 2> ctr{0x243f6a8885a308d3, 0x13198a2e03707344};
  Counter<uint64_t, 1> key{0xa4093822299f31d0};
  test_philox<uint64_t, 2, 7>(ctr, key, {0x98ed1534392bf372, 0x67528b1568882fd5});
  test_philox_min<uint64_t, 2, 7>({0xb41da69fbfefc666, 0x511e9ce1a5534056});
  test_philox_max<uint64_t, 2, 7>({0xa4696cc04462015d, 0x724782dae17169e9});
  test_philox<uint64_t, 2, 10>(ctr, key, {0x0a5e742c2997341c, 0xb0f883d38000de5d});
  test_philox_min<uint64_t, 2, 10>({0xca00a0459843d731, 0x66c24222c9a845b5});
  test_philox_max<uint64_t, 2, 10>({0x65b021d60cd8310f, 0x4d02f3222f86df20});
}

auto TestPhilox32x2()
{
  Counter<uint32_t, 4> ctr{0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344};
  Counter<uint32_t, 2> key{0xa4093822, 0x299f31d0};
  test_philox<uint32_t, 4, 7>(ctr, key, {0x4dfccaba, 0x190a87f0, 0xc47362ba, 0xb6b5242a});
  test_philox_min<uint32_t, 4, 7>({0x5f6fb709, 0x0d893f64, 0x4f121f81, 0x4f730a48});
  test_philox_max<uint32_t, 4, 7>({0x5207ddc2, 0x45165e59, 0x4d8ee751, 0x8c52f662});
  test_philox<uint32_t, 4, 10>(ctr, key, {0xd16cfe09, 0x94fdcceb, 0x5001e420, 0x24126ea1});
  test_philox_min<uint32_t, 4, 10>({0x6627e8d5, 0xe169c58d, 0xbc57ac4c, 0x9b00dbd8});
  test_philox_max<uint32_t, 4, 10>({0x408f276d, 0x41c83b0e, 0xa20bc7c6, 0x6d5451fd});
}

struct MockTrait
{
  constexpr static size_t counter_size = 4;
  constexpr static size_t key_size = 4;
  constexpr static size_t internal_key_size = 4;
  using result_type = uint32_t;
  using counter_t = Counter<uint32_t, counter_size>;
  using key_t = Counter<uint32_t, key_size>;
  using internal_key_t = Counter<uint32_t, internal_key_size>;

  static internal_key_t set_key(key_t k) noexcept { return k; }

  static counter_t bijection(counter_t ctr, [[maybe_unused]] internal_key_t key) noexcept
  {
    auto c0 = ctr[0] * 4;
    return {c0, c0 + 1U, c0 + 2U, c0 + 3U};
  }
};

auto test_mock_trait()
{
  using ctr_t = Counter<uint32_t, 4>;
  {
    ctr_t expected = {0U, 1U, 2U, 3U};
    ctr_t actual = MockTrait::bijection({0U, 0U, 0U, 0U}, {});
    assert_equal(expected, actual);
  }
  {
    ctr_t expected = {4U, 5U, 6U, 7U};
    ctr_t actual = MockTrait::bijection({1U, 0U, 0U, 0U}, {});
    assert_equal(expected, actual);
  }
  {
    ctr_t expected = {8U, 9U, 10U, 11U};
    ctr_t actual = MockTrait::bijection({2U, 0U, 0U, 0U}, {});
    assert_equal(expected, actual);
  }
}

auto test_call_operator()
{
  using T = MockTrait::result_type;
  CounterBasedGenerator<MockTrait> gen{};
  for (T i{}; i < T{1000}; ++i)
  {
    assert_equal(gen(), i);
  }
}

auto test_discard(std::vector<uint32_t> &&expected, unsigned steps)
{
  std::vector<uint32_t> actual{};
  CounterBasedGenerator<MockTrait> gen{};
  while (actual.size() < expected.size())
  {
    actual.push_back(gen());
    gen.discard(steps);
  }
  assert_equal(expected, actual);
}

auto test_discard()
{
  test_discard({0, 2, 4, 6, 8}, 1);
  test_discard({0, 8, 16, 24, 32}, 7);
}

auto test_increment_decrement(auto input, auto expected_incremented)
{
  auto t1 = input;
  auto t2 = expected_incremented;
  assert_equal(++t1, expected_incremented);
  assert_equal(--t2, input);
}

auto test_increment_decrement()
{
  using ctr1 = Counter<uint32_t, 1>;
  using ctr2 = Counter<uint32_t, 2>;
  test_increment_decrement(ctr1{1U}, ctr1{2U});
  test_increment_decrement(ctr1{0U}, ctr1{1U});
  test_increment_decrement(ctr1{UINT32_MAX}, ctr1{0U});
  test_increment_decrement(ctr2{1U, 1U}, ctr2{2U, 1U});
  test_increment_decrement(ctr2{0U, 2U}, ctr2{1U, 2U});
  test_increment_decrement(ctr2{0U, 0U}, ctr2{1U, 0U});
  test_increment_decrement(ctr2{UINT32_MAX, UINT32_MAX}, ctr2{0U, 0U});
  test_increment_decrement(ctr2{UINT32_MAX, 2U}, ctr2{0U, 3U});
}

auto test_add_small()
{
  using ctr2 = Counter<uint32_t, 2>;
  using ctr3 = Counter<uint32_t, 3>;
  {
    ctr2 input{UINT32_MAX, 1};
    ctr2 expected{1, 2};
    ctr2 actual = input + 2;
    assert_equal(actual, expected);
  }
  {
    ctr2 input{2, 1};
    ctr2 expected{4, 1};
    ctr2 actual = input + 2;
    assert_equal(actual, expected);
  }
  {
    ctr3 input{UINT32_MAX, 1, 1};
    ctr3 expected{1, 2, 1};
    ctr3 actual = input + 2;
    assert_equal(actual, expected);
  }
  {
    ctr3 input{2, 1, 1};
    ctr3 expected{4, 1, 1};
    ctr3 actual = input + 2;
    assert_equal(actual, expected);
  }
}

auto test_reinterpret()
{
//  constexpr uint32_t two16 = uint32_t{1} << 16U;
//  constexpr uint16_t w0 = 12;
//  constexpr uint16_t w1 = 34;
//  constexpr uint16_t w2 = 45;
//  constexpr uint16_t w3 = 78;
//  constexpr uint32_t v0 = w0 + w1 * two16;
//  constexpr uint32_t v1 = w2 + w3 * two16;
//
//  Counter<uint16_t, 4> ctr{w0, w1, w2, w3};
//  auto r = ctr.reinterpret<uint32_t>();
//
//  assert_equal(v0, r[0]);
//  assert_equal(v1, r[1]);
}

int main()
{
  TestThreeFry64x4();
  TestThreeFry64x2();
  TestThreeFry32x4();
  TestThreeFry32x2();
  TestPhilox32x2();
  TestPhilox32x4();
  TestPhilox64x2();
  TestPhilox64x4();
  test_mock_trait();
  test_call_operator();
  test_discard();
  test_increment_decrement();

  test_reinterpret();
  test_add_small();
  std::cout << "success\n";
  return EXIT_SUCCESS;
}
