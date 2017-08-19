#ifndef UNIT_PARSE_TOKEN_HPP_INCLUDED
#define UNIT_PARSE_TOKEN_HPP_INCLUDED

#include <quan/lexer/sourcepos.hpp>
#include <quan/atomicstring.hpp>
#include "SlkParse.h"

namespace quan_lexer{

   struct token{
      int                        m_token_id; 
      std::string                m_lexeme;   // the source comprising the lexeme
      quan::lexer::source_pos    m_position;
      quan::atomicstring<char>   m_filename;

      token(): m_token_id{UNDEFINED}, m_position(1,1){}
      std::string to_string() const;

   };
}

#endif
