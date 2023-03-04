#pragma once

#include "charMatcher.hpp"

/**
@brief compares a single char for equality
**/
template <std::equality_comparable Char>
struct charEqualityMatcher : charMatcher<Char>{
   charEqualityMatcher(Char ch)
   : mChar(ch){}
   bool operator()(Char const & ch) const override
   { return ch == mChar;};
   private:
   Char mChar;
};
