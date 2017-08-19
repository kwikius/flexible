#ifndef UNIT_PARSE_LEXER_HPP_INCLUDED
#define UNIT_PARSE_LEXER_HPP_INCLUDED

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif
#include <quan/lexer/sourcepos.hpp>
#include "src_token.hpp"

namespace quan_lexer{

   struct src_lexer : yyFlexLexer {
      src_lexer(
         std::string const & filename_in,
            std::istream * in,
            std::ostream * out
      );
      typedef  quan_lexer::src_token token_type;

      int yylex( token_type & tok);
      quan::lexer::source_pos get_position() const;
      std::ostream & out() ;
      void init_tok(token_type& tok, int tokenClass);
      void do_string_handle(token_type & tok);
      int do_string_literal(token_type& tok);
      int do_charseq(token_type& tok);
      int do_tokenID(token_type& tok);
      int do_tokenID(token_type& tok,short ID);
      int do_quoted_char(token_type& tok);

      private:
        quan::lexer::source_pos m_position;
        std::string const & m_filename;
        std::string m_current_string_literal;
   };
}

#endif
