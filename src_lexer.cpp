
#include <quan/gc.hpp>
//#include <quan/lexer/prod.hpp>
//#include <quan/lexer/term.hpp>
#include <quan/atomicstring.hpp>
#include "src_lexer.hpp"
#include "SlkParse.h"

quan_lexer::src_lexer::src_lexer(
   std::string const & filename_in,
   std::istream* in,
   std::ostream* out
)
: yyFlexLexer(in,out),m_position(1,1),m_filename(filename_in){}

quan::lexer::source_pos
quan_lexer::src_lexer::get_position() const
{ return m_position;}

std::ostream & quan_lexer::src_lexer::out()
{ return yyout;}


void quan_lexer::src_lexer::init_tok(token_type& tok)
{
   tok.m_filename = m_filename;
   tok.m_position = m_position;
   m_position.column += yyleng;
}

int quan_lexer::src_lexer::do_tokenID(token_type& tok)
{
   init_tok(tok);
   tok.m_lexeme = yytext;
   return tok.m_token_id = yytext[0];
}

int quan_lexer::src_lexer::do_tokenID(token_type& tok,short ID)
{
   init_tok(tok);
   tok.m_lexeme = yytext;
   return tok.m_token_id = ID;
}

void quan_lexer::src_lexer::do_string_handle(token_type & tok)
{
   init_tok(tok);
   tok.m_lexeme = yytext;
}

// no pos set up here as string literal is parsed variously
int quan_lexer::src_lexer::do_string_literal(token_type& tok)
{
   tok.m_filename = m_filename;
   tok.m_lexeme = m_lexeme;
   return tok.m_token_id = quan_lexer::STRING_LITERAL_;
}

int quan_lexer::src_lexer::do_charseq(token_type& tok)
{
   tok.m_filename = m_filename;
   tok.m_lexeme = m_lexeme;
   return tok.m_token_id = quan_lexer::CHARSEQ_;
}
