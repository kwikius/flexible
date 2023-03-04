#pragma once

#include "charMatcher.hpp"

/**
@brief match one from contiguous range of chars
**/
template <std::equality_comparable Char>
struct charRangeMatcher : charMatcher<Char>{

     charRangeMatcher(Char const & lowest, Char const & highest)
     : lowest{lowest},highest{highest}{
        assert( lowest <= highest);
     }

     bool operator()(Char const & ch) const override
     {
        return (ch >= lowest) && (ch <= highest);
     }
    private:
    Char const lowest;
    Char const highest;
};

