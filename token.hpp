#ifndef UNIT_PARSE_TOKEN_HPP_INCLUDED
#define UNIT_PARSE_TOKEN_HPP_INCLUDED

#include <quan/lexer/sourcepos.hpp>
#include <quan/atomicstring.hpp>
#include "token_id.hpp"

namespace quan_lexer{

   struct token{
      static constexpr int eof = 256 ;
      static constexpr int undefined = 257;
      static constexpr int error  = 258;
      int                        m_token_id; 
      std::string                m_lexeme;   // the source text comprising the lexeme
      quan::lexer::source_pos    m_position;
      quan::atomicstring<char>   m_filename;
      token(): m_token_id{undefined}, m_position(1,1){}
   };
}

#endif
