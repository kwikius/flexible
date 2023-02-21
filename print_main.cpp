
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
      while (lex.yylex(tok) != quan_lexer::eof){
         out << "---\"" << tok.filepos.filename << "\"," << tok.filepos.position << "\n";
         if ( tok.m_token_id < 256){
            out << "ID["<< tok.m_token_id << "]";
         }else{
            switch (tok.m_token_id){
               case  quan_lexer::Emit:
               case  quan_lexer::Jump:
               case  quan_lexer::Append:
               case  quan_lexer::PushFpos:
               case  quan_lexer::PopFpos:
               case  quan_lexer::Default:
               case  quan_lexer::State:
                  out << "keyword" ;
                  break;
               case  quan_lexer::DotDot:
                  out << "punct";
                  break;
               case quan_lexer::Identifier:
                  out << "name";
                  break;
               case  quan_lexer::StringLiteral:
                  out << "string_literal";
                  break;
               case  quan_lexer::CharSeq:
                  out << "charseq";
                  break;
               case  quan_lexer::Char:
                  out << "char" ;
                  break;
               case  quan_lexer::EmptySet:
                  out << "empty_set";
                  break;
               case  quan_lexer::error:
                  out << "ERROR" ;
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
