#ifndef UNIT_PARSE_TOKEN_HPP_INCLUDED
#define UNIT_PARSE_TOKEN_HPP_INCLUDED

#include <quan/lexer/sourcepos.hpp>
#include <quan/atomicstring.hpp>

namespace quan_lexer{

   struct src_token{

      static const int  classEndOfInput = 0;
      static const int  classUndefined = 1;
      static const int  classIdentifier = 2;
      static const int  classTokenID = 3;
      static const int  classStringLiteral = 4;
      static int const  classChar = 5;
      static int const  classCharSeq = 6;

      int                        token_class;
      short                      token_id; // id as used by SLK
      union{
         std::string*                   m_string_literal;
         char const *                   m_identifier;
         char                           m_char;
      };
      quan::lexer::source_pos m_position;
      quan::atomicstring<char> m_filename;
      src_token(): m_position(1,1){}
      std::string to_string() const;

   };
}

#endif
