#pragma once

#include "exprMatcher.hpp"

/**
@brief  always empty matcher
If used alone would result in an infinite loop!
TODO revisit return EOF on empty str?
**/
template <std::equality_comparable Char>
struct emptySetMatcher : exprMatcher<Char>{
    void reset() override{}
    matchState consume(std::string & strIn, std::string & lexemeOut) override
    {
       if (!strIn.empty()){
          return matchState::MatchedEmpty;
       }else{
          return matchState::Eof;
       }
    }
};
