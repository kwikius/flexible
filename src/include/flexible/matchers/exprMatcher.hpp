#pragma once

#include <string>
#include <utility>
#include <concepts>

#include "matchState.hpp"

/**
abstract base class expression matcher state machine
**/
template <std::equality_comparable Char>
struct exprMatcher{
   virtual ~exprMatcher() = default;
   virtual void reset() = 0;

   [[nodiscard]] virtual matchState
   consume(std::basic_string<Char> & strIn, std::basic_string<Char> & lexemeOut) = 0;
   protected:
   static Char pop_front (std::basic_string<Char> & str)
   {
     auto ch = str.front();
     str.erase(0,1);
     return ch;
   }

   static void backtrack(std::basic_string<Char> & strIn,std::basic_string<Char> & lexeme)
   {
      strIn = std::move(lexeme) + strIn;
   }
};
