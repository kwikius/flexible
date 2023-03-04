#pragma once

#include "exprMatcher.hpp"

/**
@brief abstract base class charMatcher matches only a single Char element
**/
template <std::equality_comparable Char>
struct charMatcher : exprMatcher<Char>{
   virtual ~charMatcher() = default;
   /**
   @brief return true if character was matched, else false
   **/
   virtual bool operator()(Char const & ch) const = 0;
   void reset() override{}

   matchState consume(std::basic_string<Char> & strIn,std::basic_string<Char> & lexemeOut) override
   {
      if ( ! strIn.empty()){
         Char const ch = this->pop_front(strIn);
         lexemeOut += ch;
         if((*this)(ch)){
            return matchState::Matched;
         }else{
            return matchState::NotMatched;
         }
      }else{
         return matchState::Eof;
      }
   }
};

