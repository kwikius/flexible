
#include <iostream>
#include <fstream>
#include <cassert>

#include "src_lexer.hpp"
#include "token_id.hpp"

bool print_toks(std::string const & filename, std::istream & in,std::ostream& out)
{
   try{
      int errorcount = 0;
      quan_lexer::src_lexer lex(filename,&in,&out);

      quan_lexer::token tok;
      while (lex.yylex(tok) != quan_lexer::token::eof){
         out << "---\"" << tok.m_filename << "\"," << tok.m_position << "\n";
         if ( tok.m_token_id < 256){
            out << "ID["<< tok.m_token_id << "]";
         }else{
            switch (tok.m_token_id){
               case  quan_lexer::EMIT_:
               case  quan_lexer::JUMP_:
               case  quan_lexer::APPEND_:
               case  quan_lexer::PUSHFPOS_:
               case  quan_lexer::POPFPOS_:
               case  quan_lexer::DEFAULT_:
               case  quan_lexer::STATE_:
                  out << "keyword" ;
                  break;
               case  quan_lexer::DOT_DOT_:
                  out << "punct";
                  break;
               case quan_lexer::IDENTIFIER:
                  out << "name";
                  break;
               case  quan_lexer::STRING_LITERAL_:
                  out << "string_literal";
                  break;
               case  quan_lexer::CHARSEQ_:
                  out << "charseq";
                  break;
               case  quan_lexer::CHAR:
                  out << "char" ;
                  break;
               case  quan_lexer::EMPTY_SET:
                  out << "empty_set";
                  break;
               case  quan_lexer::token::error:
                  out << "ERROR" ;
                  ++errorcount;
                  break;
               case  quan_lexer::token::undefined:
                  out << "undefined" ;
                  ++errorcount;
                  break;
               default:
                  out << "UNKNOWN"; 
                  ++errorcount;
                  break;
            }
         }
         out << ": " << tok.m_lexeme << "\n\n";
      }

      if( errorcount==0){
         std::cout << "\n******** NO ERRORS ********\n";
      }
      else{
         std::cout << "\n######### " << errorcount << " ERRORS ##########\n";
      }
      return true;
   }
   catch(std::exception & e){
      std::cout << "Exception " << e.what() << '\n';
      return false;
   }
   catch(...){
      std::cout << "Caught unknown Exception\n";
      return false;
   }
}



int print_main (int argc, char *argv[])
{
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
   print_toks(argv[1],in,*out);
   std::ofstream * fout = dynamic_cast<std::ofstream*>(out);
   if ( fout){
      fout->close();
      delete fout;
   }
   return EXIT_SUCCESS;
}