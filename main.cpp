
#include <quan/gc.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
//#include "reflex.hpp"
//#include "flexer.hpp"
//#include "lexeme.hpp"

#include "src_lexer.hpp"
#include "SlkParse.h"

/*
int do_parse(std::string const & filename, std::ostream* out)
{
   assert(out !=0);
   try{
   quan_lexer::reflex src_flex(filename,out);
   int result = src_flex.get_num_errors() ;
#if (1)
   if (result == 0){
      std::cout << "Built lexer .. starting use ****************************\n\n";
      std::string lexer_data = "data2.txt";
      std::cout << "lexing \"" << lexer_data << "\"\n";
     
      quan_lexer::flexer*  flex = src_flex.get_flexer(lexer_data,out);
      quan_lexer::lexeme lexeme;
      int lexresult;
      for ( lexresult = flex->yylex(lexeme) ; lexresult == 1; lexresult = flex->yylex(lexeme) ){
       *out << lexeme.m_source_pos << " " << lexeme.m_token_name << " = " << lexeme.m_lexeme <<'\n';
      }
      if( lexresult != -1){
        std::cout << "failed to parse data\n";
        *out << "failed to parse data\n";
        result = 1;
      }
   }
#endif
   return result;
   }
    catch ( std::exception & e){

     std::cout << "Exception " << e.what() << "\n";
     return -1;
   }
}
*/

int main (int argc, char *argv[])
{
  // quan::gc_init();

   try{
      if(argc == 1){
         printf("-- No source file specified\n");
         return EXIT_FAILURE;
      }
      std::ifstream in(argv[1]);

      if(!in){
         std::cout << "couldnt open \"" << argv[1] << "\"\n";
         return EXIT_FAILURE;
      }
      std::ostream * out = 0;

      if( argc > 2){
         std::cout << "output to file\n";
         out = new std::ofstream(argv[2]);
         if(!(*out)){
            std::cout << "couldnt open \"" << argv[2] << "\"\n";
            if (out){
               delete out;
            }
            return EXIT_FAILURE;
         }
      }else{
         out = &std::cout;
      }
      int errorcount = 0;
      // stop processing files once threshold exceeded
      int const threshold = 5;


      quan_lexer::src_lexer lex(argv[1],&in,out);

      quan_lexer::src_token tok;
      while (lex.yylex(tok) != quan_lexer::_Slk_token_defines::END_OF_SLK_INPUT_){
         *out << "---- " << tok.m_position << " ----\n";
         *out << "class = " ;
         switch (tok.token_class){
            
            case  quan_lexer::src_token::classUndefined:
               *out << "undefined";
               break;
            case  quan_lexer::src_token::classIdentifier:
               *out << "indentifier, text = \"";
                *out << tok.m_identifier << "\"";
               break;
            case  quan_lexer::src_token::classTokenID:
               *out << "ID  \"" << tok.token_id << "\"";
               break;
            case  quan_lexer::src_token::classStringLiteral:
               *out << "string literal, text = \"" << *tok.m_string_literal << "\"";
               break;
            case  quan_lexer::src_token::classChar:
               *out << "char, text = \'" << tok.m_char << "\'";
               break;
            case  quan_lexer::src_token::classCharSeq:
               *out <<"charseq, tetxt = \'" << *tok.m_string_literal << "\'";
               break;
            default:
               *out << "UNKNOWN CLASS \"" << tok.token_class << "\"";
 
               break;
         }
         *out << "\n\n";
       //  *out << "tok id = " << tok.token_id <<'\n';
         
      }

      if(out != &std::cout){
         delete out;
      }
      if( errorcount==0){
         std::cout << "\n******** NO ERRORS ********\n";
      }
      else{
         std::cout << "\n######### " << errorcount << " ERRORS ##########\n";
      }
      return 0;
   }
   catch(std::exception & e){
      std::cout << "Exception " << e.what() << '\n';
   }
   catch(...){
      std::cout << "Caught unknown Exception\n";
   }
}




