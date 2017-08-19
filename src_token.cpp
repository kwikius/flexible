
#include "SlkString.h"
#include "src_token.hpp"

std::string quan_lexer::src_token::to_string()const
{
   switch(token_class){
      case classIdentifier:
         return m_lexeme.to_c_str();
      case classTokenID:
        return "ID";
     // return quan_lexer::SlkGetSymbolName(this->token_id);
      case classStringLiteral:
         return m_lexeme.to_c_str();
      case classUndefined : // string
         return "Unexpected";
      default:
         return "bad token";
   }

}
