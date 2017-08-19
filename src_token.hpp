#ifndef UNIT_PARSE_TOKEN_HPP_INCLUDED
#define UNIT_PARSE_TOKEN_HPP_INCLUDED

#include <quan/lexer/sourcepos.hpp>
#include <quan/atomicstring.hpp>

namespace quan_lexer{

   struct src_token{

      static int constexpr  classEndOfInput = 0;
      static int constexpr  classUndefined = 1;
      static int constexpr  classIdentifier = 2;
      static int constexpr  classTokenID = 3;
      static int constexpr  classStringLiteral = 4;
      static int constexpr  classChar = 5;
      static int constexpr  classCharSeq = 6;

      int                        token_class;
      short                      token_id; // id as used by SLK
      quan::atomicstring<char>  m_lexeme;

      quan::lexer::source_pos    m_position;
      quan::atomicstring<char>   m_filename;

      src_token(): m_position(1,1){}
      std::string to_string() const;

   };
}

#endif
