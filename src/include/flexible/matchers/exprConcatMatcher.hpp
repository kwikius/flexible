#ifndef EXPRCONCATMATCHER_HPP_INCLUDED
#define EXPRCONCATMATCHER_HPP_INCLUDED

#include <vector>
#include <memory>
#include <cassert>

#include "exprMatcher.hpp"

/**
 @brief Concatenation: match a sequence of exprMatchers
**/
template <std::equality_comparable Char>
struct exprConcatMatcher : exprMatcher<Char>{
   exprConcatMatcher() : currentMatchIndex{0}{}

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
      currentMatchIndex = 0;
   }

   [[nodiscard]] matchState
   consume(std::basic_string<Char> & strIn, std::basic_string<Char> & lexemeOut) override
   {
      if ( this->mSubMatchers.empty()){
         return matchState::NotMatched;
      }
      if ( !strIn.empty()){
         for (;;){
            switch(auto state = this->mSubMatchers[currentMatchIndex]->consume(strIn,lexemeOut)){
               case matchState::Matched:
                  [[fallthrough]];
               case matchState::MatchedEmpty:
                  if (++currentMatchIndex < std::ssize(mSubMatchers)){
                     break; // onto next matcher
                  }
                  [[fallthrough]];
               case matchState::NotMatched:
                  this->reset();
                  return state;
               default:
                  assert(false);
            }
         }
      }else{
          return matchState::Eof;
      }
   }
   private:
   std::vector<std::unique_ptr<exprMatcher<Char> > > mSubMatchers;
   int currentMatchIndex;
};

#endif // EXPRCONCATMATCHER_HPP_INCLUDED
