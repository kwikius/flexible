
#include <sstream>
#include <string>
#include <iostream>

#include <quan_matters/test/simple_test.hpp>

#include "src_lexer.hpp"
#include "token_id.hpp"

int errors = 0;

namespace {
   /*
      
   */
   std::string test1_str = "hello";
   std::string test1_str1 = "\t hello \n";
   void test1(quan_lexer::token const & tok, int iter)
   {
       switch (iter){
          case 0:
             QUAN_CHECK(tok.m_token_id == quan_lexer::IDENTIFIER);
             QUAN_CHECK(tok.m_lexeme == test1_str );
          break;
          default:
             QUAN_CHECK(false && "shouldnt get here");
          break;
       }
   }
   //------------------------
   
   
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

   EPILOGUE
}