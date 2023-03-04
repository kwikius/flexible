#pragma once

#include <memory>
#include "exprMatcher.hpp"

/*
 @brief match a string or the empty set
*/
template <std::equality_comparable Char>
struct exprOptionalMatcher : exprMatcher<Char>{

    template <typename T>
    requires std::derived_from<T,exprMatcher<Char> >
    exprOptionalMatcher(std::unique_ptr<T> && p)
    : matcher(std::move(p)){}
    void reset()override
    { this->matcher->reset();}

   [[nodiscard]] matchState
   consume(std::string & strIn, std::string & lexemeOut) override
   {
       if (!strIn.empty()){
          switch (auto const result = matcher->consume(strIn,lexemeOut)){
             case matchState::Matched:
                return result;
             case matchState::NotMatched:
                this->backtrack(strIn,lexemeOut);
                [[fallthrough]];
             case matchState::MatchedEmpty:
                return matchState::MatchedEmpty;
             default:
               assert(false);
               return result;
          }
      }else{
         return matchState::Eof;
      }
   }
    private:
    std::unique_ptr<exprMatcher<Char> > matcher;
};
