#ifndef UNIT_PARSE_TOKEN_HPP_INCLUDED
#define UNIT_PARSE_TOKEN_HPP_INCLUDED

#include <quan/lexer/filepos.hpp>
#include <quan/atomicstring.hpp>
#include "token_id.hpp"

namespace quan_lexer{

   inline constexpr int eof = 0 ;
   inline constexpr int error  = -1;

   struct token{

      int                        m_token_id;
      std::string                m_lexeme;   // the source text comprising the lexeme
      quan::lexer::filepos       filepos;
      token()
      : m_token_id{error}
      ,filepos{quan::atomicstring<char>{},{1,1}}
      {}
   };
}

#endif
