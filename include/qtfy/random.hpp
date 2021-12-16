#ifndef QTFY_RANDOM_HPP
#define QTFY_RANDOM_HPP

#include "qtfy/random/counter.hpp"
#include "qtfy/random/counter_based_engine.hpp"
#include "qtfy/random/philox_trait.hpp"
#include "qtfy/random/threefry_trait.hpp"
#include "qtfy/random/utlities.hpp"

namespace qtfy::random {

template <class word_t, unsigned words, unsigned rounds, class result_t = word_t>
using threefry_engine = counter_based_engine<threefry_trait<word_t, words, rounds>, result_t>;

template <class word_t, unsigned words, unsigned rounds, class result_t = word_t>
using philox_engine = counter_based_engine<philox_trait<word_t, words, rounds>, result_t>;

}  // namespace qtfy::random

#endif
