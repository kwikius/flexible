
#include <quan/gc.hpp>
#include <quan/atomicstring.hpp>
#include "src_lexer.hpp"
#include "token_id.hpp"

quan_lexer::src_lexer::src_lexer(
   std::string const & filename_in,
   std::istream* in,
   std::ostream* out
)
: yyFlexLexer{in,out}
 ,m_position{1,1}
 ,m_filename{filename_in}
{}

quan::lexer::source_pos
quan_lexer::src_lexer::get_position() const
{ return m_position;}

std::ostream & quan_lexer::src_lexer::out()
{ return yyout;}


void quan_lexer::src_lexer::init_tok(token_type& tok)
{
   tok.filepos = {m_filename,m_position};
   tok.m_lexeme = yytext;
   m_position.column += yyleng;
}

int quan_lexer::src_lexer::do_tokenID(token_type& tok)
{
   init_tok(tok);
   return tok.m_token_id = yytext[0];
}

int quan_lexer::src_lexer::do_tokenID(token_type& tok,short ID)
{
   init_tok(tok);
   return tok.m_token_id = ID;
}

int quan_lexer::src_lexer::do_identifier(token_type & tok)
{
   init_tok(tok);
   return tok.m_token_id = quan_lexer::Identifier;
}

// no pos set up here as string literal is parsed variously
int quan_lexer::src_lexer::do_string_literal(token_type& tok)
{
   switch(m_lexeme.size()){
      case 0:
         tok.m_token_id = quan_lexer::EmptySet  ;
         break;
      case 1:
         tok.m_token_id = quan_lexer::Char;
         break;
      default:
         tok.m_token_id = quan_lexer::StringLiteral;
         break;
   }
   tok.filepos.filename = m_filename;
   tok.m_lexeme = m_lexeme;
   return tok.m_token_id;
}

int quan_lexer::src_lexer::do_charseq(token_type& tok)
{
   switch(m_lexeme.size()){
      case 0:
         tok.m_token_id = quan_lexer::EmptySet ;
         break;
      case 1:
         tok.m_token_id = quan_lexer::Char;
         break;
      default:
         tok.m_token_id = quan_lexer::CharSeq;
         break;
   }
   tok.filepos.filename = m_filename;
   tok.m_lexeme = m_lexeme;
   return tok.m_token_id;
}

void quan_lexer::src_lexer::do_error(token_type& tok, const char* msg)
{
   tok.filepos = {m_filename,m_position};
   tok.m_lexeme = '"';
   tok.m_lexeme += yytext;
   tok.m_lexeme += "\" " ;
   tok.m_lexeme +=  msg;

}
