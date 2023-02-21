

#ifndef FLEXIBLE_TOKEN_ID_HPP
#define FLEXIBLE_TOKEN_ID_HPP

namespace quan_lexer {

   /*
      Tokens are integers
      Any value less than 256 may be used for char ids
      value 256 represenst end of input
      value 257 represents undefined
      values below 300 are reserved
   */

   enum TokenID {
     Identifier = 300
   , Emit
   , Jump
   , Append
   , PushFpos
   , PopFpos
   , Default
   , State
   , StringLiteral
   , DotDot
   , CharSeq
   , Char
   , EmptySet
   };
}


#endif //FLEXIBLE_TOKEN_ID_HPP
