#ifndef QUAN_LEXER_FLEXER_HPP_INCLUDED
#define QUAN_LEXER_FLEXER_HPP_INCLUDED

// #include <quan/gc.hpp>

#include <iostream>
#include <string>
#include <stack>
#include <quan/lexer/sourcepos.hpp>

namespace quan_lexer{

   struct lexeme;
   struct mode;

   struct flexer{

      flexer (std::string const & src_filename,std::ostream * out);

      // returns 0 on eof
      int yylex( quan_lexer::lexeme& tok);

      std::string                         m_filename;
      std::istream *                      m_istream;
      std::ostream *                      m_ostream;
      quan_lexer::mode*                   m_mode;
      std::size_t                         m_buf_length;
      quan::lexer::source_pos             m_source_pos;
      std::stack<quan::lexer::source_pos> m_source_pos_stack;

   };

}
#endif // QUAN_LEXER_FLEXER_HPP_INCLUDED
