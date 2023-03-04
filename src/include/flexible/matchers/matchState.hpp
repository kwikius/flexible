#pragma once

/**
  @brief enum returned as result of a match
**/
enum class matchState{
/**
    @brief  EOF empty strIn
      strIn  state empty
      Lexeme state unchanged
**/
   Eof,
/**
    @brief No match looking ahead into strIn
    strIn state attemppted match characters after last successful match removed
    Lexeme state attempted match characters after last successful match added
**/
   NotMatched,
/**
   @brief match
   strIn state  matched characters removed
   lexeme state The resulting lexeme
**/
   Matched,
/**
   @brief Empty match
   strIn  Unchanged
   lexeme Empty
**/
   MatchedEmpty
};

