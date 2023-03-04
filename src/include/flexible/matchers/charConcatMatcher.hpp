#pragma once

#include "exprMatcher.hpp"

/**
  @brief match a simple string
  @todo can be better done just iterate the string
**/
template <std::equality_comparable Char>
struct charConcatMatcher : exprMatcher<Char> {

    charConcatMatcher(std::basic_string<Char> const & seq)
    : m_str{seq}{}

     [[nodiscard]] matchState
     consume(std::basic_string<Char> & strIn, std::basic_string<Char> & lexemeOut) override
     {
        if (!strIn.empty()){
           auto const strInSize = strIn.size();
           auto const mStrSize = m_str.size();
           if ( strInSize >= mStrSize){
                for ( std::size_t i = 0U; i< mStrSize;++i){
                  Char const ch = this->pop_front(strIn);
                  lexemeOut += ch;
                  if  (m_str[i] != ch){
                     return matchState::NotMatched;
                  }
                }
                return matchState::Matched;
           }else{ // to match semantics it is desirable to move the string to add to the lexeme
                 // so it can be used for error messages etc.
                // if so the temporary seems required  here ( so += doesnt work)
                lexemeOut = lexemeOut + std::move(strIn);
                return matchState::NotMatched;
           }
        }else{
           return matchState::Eof;
        }
     }
    void reset()override
    {}
   private:
   std::basic_string<Char> const m_str;
};
