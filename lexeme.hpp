#ifndef QUAN_LEXER_LEXEME_HPP_INCLUDED
#define QUAN_LEXER_LEXEME_HPP_INCLUDED

#include <string>
#include <quan/atomicstring.hpp>
#include <quan/lexer/sourcepos.hpp>

namespace quan_lexer{

 // the returned lexeme
  //rename to token?
  struct lexeme{
      lexeme(): m_source_pos(0,0){}
      quan::atomicstring<char>       m_token_name;  // Unique ID for the token
      std::string                    m_lexeme;      // the string from the stream
      quan::atomicstring<char>       m_filename;    // filename of the source
      quan::lexer::source_pos        m_source_pos;  // position in the file
  };

}

#endif // QUAN_LEXER_LEXEME_HPP_INCLUDED
