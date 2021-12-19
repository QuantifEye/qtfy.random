#ifndef QTFY_RANDOM_HPP
#define QTFY_RANDOM_HPP

#include "qtfy/random/counter.hpp"
#include "qtfy/random/counter_based_engine.hpp"
#include "qtfy/random/philox_trait.hpp"
#include "qtfy/random/threefry_trait.hpp"
#include "qtfy/random/utlities.hpp"

namespace qtfy::random {

template<unsigned rounds = 20, class return_t = uint64_t>
using threefry2x64 = counter_based_engine<threefry2x64_trait<rounds>, return_t>;

template<unsigned rounds = 20, class return_t = uint32_t>
using threefry2x32 = counter_based_engine<threefry2x32_trait<rounds>, return_t>;

template<unsigned rounds = 20, class return_t = uint64_t>
using threefry4x64 = counter_based_engine<threefry4x64_trait<rounds>, return_t>;

template<unsigned rounds = 20, class return_t = uint32_t>
using threefry4x32 = counter_based_engine<threefry4x32_trait<rounds>, return_t>;

template<unsigned rounds = 10, class return_t = uint64_t>
using philox2x64 = counter_based_engine<philox2x64_trait<rounds>, return_t>;

template<unsigned rounds = 10, class return_t = uint32_t>
using philox2x32 = counter_based_engine<philox2x32_trait<rounds>, return_t>;

template<unsigned rounds = 10, class return_t = uint64_t>
using philox4x64 = counter_based_engine<philox4x64_trait<rounds>, return_t>;

template<unsigned rounds = 10, class return_t = uint32_t>
using philox4x32 = counter_based_engine<philox4x32_trait<rounds>, return_t>;

}  // namespace qtfy::random

#endif
