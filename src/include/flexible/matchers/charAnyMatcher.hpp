#ifndef CHRAANYMATCHER_HPP_INCLUDED
#define CHRAANYMATCHER_HPP_INCLUDED

#include "charMatcher.hpp"

/**
@brief match any char
**/
template <std::equality_comparable Char>
struct charAnyMatcher : charMatcher<Char>{
   bool operator()(Char const & ch) const override
   { return true;};
};

#endif // CHRAANYMATCHER_HPP_INCLUDED
