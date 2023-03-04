#pragma once

#include <ranges>

#include "charMatcher.hpp"

/**
@brief match one of a list of chars
**/
template <std::equality_comparable Char>
struct charAlternateMatcher : charMatcher<Char>{
   charAlternateMatcher(std::basic_string<Char> s)
   :symbols{s}{}

   bool operator()(Char const & ch) const override
   {
      return std::ranges::find(symbols,ch) != symbols.end();
   }
   private:
   std::basic_string<Char> symbols;
};


