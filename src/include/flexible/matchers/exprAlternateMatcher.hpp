#pragma once

#include <vector>
#include <memory>

#include "exprMatcher.hpp"

template <std::equality_comparable Char>
struct exprAlternateMatcher : exprMatcher<Char>{

   template <typename T>
      requires std::derived_from<T,exprMatcher<Char> >
   void push_back(std::unique_ptr<T> && p)
   {
      mSubMatchers.push_back(std::move(p));
   }

   void reset() override
   {
      for ( auto & m : mSubMatchers){
         m->reset();
      }
   }

   [[nodiscard]] matchState consume(std::string & strIn, std::string & lexemeOut) override
   {
      if (! strIn.empty()){
         std::size_t matcherIdx = 0U;  // current subMatcher
         bool matchedEmpty = false; // keep track of subMatchers which matchedEmpty
         for ( auto & m : mSubMatchers){
            ++matcherIdx;
               switch(m->consume(strIn,lexemeOut)){
                  case matchState::MatchedEmpty:
                     matchedEmpty = true;
                     [[fallthrough]];
                  case matchState::NotMatched:
                     if (matcherIdx == mSubMatchers.size()){
                        // This was the last matcher
                        this->reset(); // always reset before return
                        if (matchedEmpty == true){
                           //one or more matchers matched empty
                           this->backtrack(strIn,lexemeOut);
                           return matchState::MatchedEmpty;
                        }
                        else {
                           return matchState::NotMatched;
                        }
                     }else{ // not last matcher
                        this->backtrack(strIn,lexemeOut);
                        break;
                     }
                  case matchState::Matched:
                     this->reset(); // always reset before return
                     return matchState::Matched;
                  default:
                     assert(false);
               }
            }
         assert(false);
         return matchState::NotMatched;
      }else{
         return matchState::Eof;
      }
   }

  private:
    std::vector<std::unique_ptr<exprMatcher<Char> > > mSubMatchers;
};

