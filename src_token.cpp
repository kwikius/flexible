
#include "SlkString.h"
#include "src_token.hpp"

std::string quan_lexer::src_token::to_string()const
{
   switch(token_class){
      case classIdentifier:
         return m_identifier;
      case classTokenID:
        return "ID";
     // return quan_lexer::SlkGetSymbolName(this->token_id);
      case classStringLiteral:
         return *m_string_literal;
      case classUndefined : // string
         return "Unexpected";
      default:
         return "bad token";
   }

}
