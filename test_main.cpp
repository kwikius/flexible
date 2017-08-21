
#include <sstream>
#include <string>
#include <iostream>

#include <quan_matters/test/simple_test.hpp>

#include "src_lexer.hpp"
#include "token_id.hpp"
#include "token.hpp"

int errors = 0;

namespace {
   /*
      these identifier strings should all return hello
   */
   char const test1_str[] = "hello";
   char const test1_str1[] = "\t hello \n";
   char const test1_str2[] = "\n \t/* a comment \n \n */ hello // another comment\n";
   void test1(quan_lexer::token const & tok, int iter)
   {
       switch (iter){
          case 0:
             QUAN_CHECK(tok.m_token_id == quan_lexer::IDENTIFIER)
             QUAN_CHECK(tok.m_lexeme == test1_str )
          break;
          default:
             QUAN_CHECK(false && "shouldnt get here")
          break;
       }
   }

   //test  keywords
   char const test2_str1[] = " emit jump append pushfpos popfpos default state ";
   void test2(quan_lexer::token const & tok, int iter)
   {
       switch (iter){
          case 0:
             QUAN_CHECK(tok.m_token_id == quan_lexer::EMIT_)
             QUAN_CHECK(tok.m_lexeme == "emit" )
          break;
          case 1:
             QUAN_CHECK(tok.m_token_id == quan_lexer::JUMP_)
             QUAN_CHECK(tok.m_lexeme == "jump" )
          break; 
          case 2:
             QUAN_CHECK(tok.m_token_id == quan_lexer::APPEND_)
             QUAN_CHECK(tok.m_lexeme == "append" )
          break;
          case 3:
             QUAN_CHECK(tok.m_token_id == quan_lexer::PUSHFPOS_)
             QUAN_CHECK(tok.m_lexeme == "pushfpos" )
          break;
          case 4:
             QUAN_CHECK(tok.m_token_id == quan_lexer::POPFPOS_)
             QUAN_CHECK(tok.m_lexeme == "popfpos" )
          break; 
          case 5:
             QUAN_CHECK(tok.m_token_id == quan_lexer::DEFAULT_)
             QUAN_CHECK(tok.m_lexeme == "default" )
          break;
          case 6:
             QUAN_CHECK(tok.m_token_id == quan_lexer::STATE_)
             QUAN_CHECK(tok.m_lexeme == "state" )
          break; 
          default:
             QUAN_CHECK(false && "shouldnt get here")
          break;
       }
   }

   // test punct
   char const test3_str1 [] = "( ) { } [ ] , ;";
   char const test3_str2 [] = "(){}[],;";
   void test3(quan_lexer::token const & tok, int iter)
   {
       switch (iter){
          case 0:
             QUAN_CHECK(tok.m_token_id == '(')
             QUAN_CHECK(tok.m_lexeme == "(" )
          break;
          case 1:
             QUAN_CHECK(tok.m_token_id == ')')
             QUAN_CHECK(tok.m_lexeme == ")" )
          break; 
          case 2:
             QUAN_CHECK(tok.m_token_id == '{')
             QUAN_CHECK(tok.m_lexeme == "{" )
          break;
          case 3:
             QUAN_CHECK(tok.m_token_id == '}')
             QUAN_CHECK(tok.m_lexeme == "}" )
          break;
          case 4:
             QUAN_CHECK(tok.m_token_id == '[')
             QUAN_CHECK(tok.m_lexeme == "[" )
          break; 
          case 5:
             QUAN_CHECK(tok.m_token_id == ']')
             QUAN_CHECK(tok.m_lexeme == "]" )
          break;
          case 6:
             QUAN_CHECK(tok.m_token_id == ',')
             QUAN_CHECK(tok.m_lexeme == "," )
          break; 
          case 7:
             QUAN_CHECK(tok.m_token_id == ';')
             QUAN_CHECK(tok.m_lexeme == ";" )
          break; 
          default:
             QUAN_CHECK(false && "shouldnt get here")
          break;
       }
   }

   // test operators
   const char test4_str1[] = " + * ? | & ! ^ . $ ";
   const char test4_str2[] = "+*?|&!^.$";
   void test4(quan_lexer::token const & tok, int iter)
   {
       switch (iter){
          case 0:
             QUAN_CHECK(tok.m_token_id == '+')
             QUAN_CHECK(tok.m_lexeme == "+" )
          break;
          case 1:
             QUAN_CHECK(tok.m_token_id == '*')
             QUAN_CHECK(tok.m_lexeme == "*" )
          break; 
          case 2:
             QUAN_CHECK(tok.m_token_id == '?')
             QUAN_CHECK(tok.m_lexeme == "?" )
          break;
          case 3:
             QUAN_CHECK(tok.m_token_id == '|')
             QUAN_CHECK(tok.m_lexeme == "|" )
          break; 
          case 4:
             QUAN_CHECK(tok.m_token_id == '&')
             QUAN_CHECK(tok.m_lexeme == "&" )
          break;
          case 5:
             QUAN_CHECK(tok.m_token_id == '!')
             QUAN_CHECK(tok.m_lexeme == "!" )
          break; 
          case 6:
             QUAN_CHECK(tok.m_token_id == '^')
             QUAN_CHECK(tok.m_lexeme == "^" )
          break; 
          case 7:
             QUAN_CHECK(tok.m_token_id == '.')
             QUAN_CHECK(tok.m_lexeme == "." )
          break; 
          case 8:
             QUAN_CHECK(tok.m_token_id == '$')
             QUAN_CHECK(tok.m_lexeme == "$" )
          break;
          default:
             QUAN_CHECK(false && "shouldnt get here")
          break;
       }
   }

   // check no holes in undefined characters
    char const undefined_chars[] = 
         "\x01\x02\x03\x04\x05\x06\x07\x08\x0E\x0F"
         "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x1E\x1F"
         "#%-/0123456789:<>\x40\x5C\x60\x7F";
         
    void test5(quan_lexer::token const & tok, int iter)
    {
        QUAN_CHECK(tok.m_token_id == quan_lexer::token::error)
    }

    // charescapes
    // looks more complex since in c++ it is required to be quoted
    // first the c style string is within dquots
    // then every escape char requires quoting
    // and some quotes also require quoting

    char const char_escapes1 [] = "\"\\'\\\"\\a\\b\\f\\n\\r\\t\\v\"";
    /*
         c++ strips away outer dquots --> "\\'\\\"\\a\\b\\f\\n\\r\\t\\v\"
         c++ replaces escapes    --> \'\"\a\b\f\n\r\t\v   
         For a string the initial escape on squot is not necessary though:
                          --> '\"\a\b\f\n\r\t\v
    */
    char const char_escapes2 [] = "\"'\\\"\\a\\b\\f\\n\\r\\t\\v\"";
    void test6 (quan_lexer::token const & tok, int iter)
    {
       QUAN_CHECK(tok.m_token_id == quan_lexer::STRING_LITERAL_)
       QUAN_CHECK(tok.m_lexeme == "\'\"\a\b\f\n\r\t\v")
       // the escape ahead of the squot is ok but not necessary
       // so this is the same string
       QUAN_CHECK(tok.m_lexeme == "'\"\a\b\f\n\r\t\v")
    }

    char const numeric_escapes1 [] = "\"a\\x(62)\\d(99)\\B(01100100)e\"";
    void test7 (quan_lexer::token const & tok, int iter)
    {
       QUAN_CHECK(tok.m_token_id == quan_lexer::STRING_LITERAL_)
       QUAN_CHECK(tok.m_lexeme == "abcde")
    }
}

void do_test(std::string const & str, void (*test_fun)(quan_lexer::token const & tok, int))
{
   std::istringstream in(str);

   quan_lexer::src_lexer lex(str,&in,&std::cout);

   quan_lexer::token tok;
   int iter = 0;
   while (lex.yylex(tok) != quan_lexer::token::eof){
      test_fun(tok, iter);
      ++iter;
   }
}

int test_main (int argc, char *argv[])
{
   do_test(test1_str,test1);
   do_test(test1_str1,test1);
   do_test(test1_str2,test1);
   do_test(test2_str1,test2);
   do_test(test3_str1,test3);
   do_test(test3_str2,test3);
   do_test(test4_str1,test4);
   do_test(test4_str2,test4);
   do_test(undefined_chars,test5); 
   do_test(char_escapes1,test6); 
   do_test(char_escapes2,test6);
   do_test(numeric_escapes1,test7);
  
   EPILOGUE
}