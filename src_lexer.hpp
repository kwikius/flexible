#ifndef UNIT_PARSE_LEXER_HPP_INCLUDED
#define UNIT_PARSE_LEXER_HPP_INCLUDED

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif
#include <quan/lexer/sourcepos.hpp>
#include "token.hpp"
#include <iostream>

namespace quan_lexer{

   struct src_lexer : yyFlexLexer {
      src_lexer(
         std::string const & filename_in,
         std::istream * in,
         std::ostream * out
      );
      
      typedef  quan_lexer::token token_type;
      int yylex( token_type & tok);
      quan::lexer::source_pos get_position() const;
      std::ostream & out() ;
    private:
      void init_tok(token_type& tok);
      int do_identifier(token_type & tok);
      int do_string_literal(token_type& tok);
      int do_charseq(token_type& tok);
      int do_tokenID(token_type& tok);
      int do_tokenID(token_type& tok,short ID);
      void do_error(token_type& tok, const char* msg);
      quan::lexer::source_pos m_position; // keep track of the current file and position
      std::string const & m_filename;
      std::string m_lexeme; // 
   };
}

#endif
