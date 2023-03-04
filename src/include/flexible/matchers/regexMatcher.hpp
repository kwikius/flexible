#pragma once

#include <quan_matters/test/test.hpp>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <list>
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <quan/meta/is_element_of.hpp>

enum class matchState{
   Eof,         // eof encountered while matching . Maybe same as NotMatched?
   NotMatched,  // ch is not a match so expression not matched
   Matched,  // ch is a match and all elements matched "matched"
   MatchedEmpty // matched the empty set ( so don't consume ch)
};

/**
abstract base class expression matcher stat machine
**/
template <std::equality_comparable Char>
struct exprMatcher{
   virtual ~exprMatcher() = default;
   virtual void reset() = 0;

   [[nodiscard]] virtual matchState
   consume(std::basic_string<Char> & strIn, std::basic_string<Char> & lexemeOut) = 0;
   protected:
   static Char pop_front (std::basic_string<Char> & str)
   {
     auto ch = str.front();
     str.erase(0,1);
     return ch;
   }

};

/**
@brief abstract base class prim matcher matches only one element
**/
template <std::equality_comparable Char>
struct primMatcher : exprMatcher<Char>{
   virtual ~primMatcher() = default;
   /**
   @brief return true if character was matched, else false
   **/
   virtual bool operator()(Char const & ch) const = 0;
   void reset() override{}

   matchState consume(std::basic_string<Char> & strIn,std::basic_string<Char> & lexemeOut) override
   {
      if ( ! strIn.empty()){
         Char const ch = this->pop_front(strIn);
         lexemeOut += ch;
         if((*this)(ch)){
            return matchState::Matched;
         }else{
            return matchState::NotMatched;
         }
      }else{
         return matchState::Eof;
      }
   }
};
/**
@brief matches a single char
**/
template <std::equality_comparable Char>
struct charMatcher : primMatcher<Char>{
   charMatcher(Char ch)
   : mChar(ch){}
   bool operator()(Char const & ch) const override
   { return ch == mChar;};
   private:
   Char mChar;
};

/**
@brief  always empty matcher
If used alone would result in an infinite loop!
TODO revisit return EOF on empty str?
**/
template <std::equality_comparable Char>
struct emptyMatcher : exprMatcher<Char>{
    void reset() override{}
    matchState consume(std::string & strIn, std::string & lexemeOut) override
    {
       if (!strIn.empty()){
          return matchState::MatchedEmpty;
       }else{
          return matchState::Eof;
       }
    }
};

/**
 @brief Concatenation: match a sequence of exprMatchers
**/
template <std::equality_comparable Char>
struct matchSequence : exprMatcher<Char>{
   matchSequence() : currentMatchIndex{0}{}

   template <typename T>
      requires std::derived_from<T,exprMatcher<Char> >
   void push_back(std::unique_ptr<T> && p)
   {
      m_matchSeq.push_back(std::move(p));
   }
   void reset() override
   {
      for ( auto & m : m_matchSeq){
         m->reset();
      }
      currentMatchIndex = 0;
   }

   [[nodiscard]] matchState consume(std::basic_string<Char> & strIn, std::basic_string<Char> & lexemeOut) override
   {
      if ( this->m_matchSeq.empty()){
         return matchState::NotMatched;
      }
      if ( !strIn.empty()){
         for (;;){
            switch(this->m_matchSeq[currentMatchIndex]->consume(strIn,lexemeOut)){
               case matchState::Matched:
                  if (++currentMatchIndex < std::ssize(m_matchSeq)){
                     break;
                  }else{
                     this->reset();
                     return matchState::Matched;
                  }
               case matchState::MatchedEmpty:
                  if (++currentMatchIndex < std::ssize(m_matchSeq)){
                     break; // dont consume char go round again
                  }else{
                     strIn = std::move(lexemeOut) + strIn;
                     this->reset();
                     return matchState::MatchedEmpty;
                  }
               case matchState::NotMatched:
                  this->reset();
                  return matchState::NotMatched;
               case matchState::Eof:
                  this->reset();
                  return matchState::Eof;
               default:
                  assert(false);
            }
         }
      }else{
          return matchState::Eof;
      }
   }
   private:
   std::vector<std::unique_ptr<exprMatcher<Char> > > m_matchSeq;
   int currentMatchIndex;
};

/**
  @brief match a simple string
  @todo can be better done just iterate the string
**/
template <std::equality_comparable Char>
struct simpleStringMatcher : matchSequence<Char>{

    simpleStringMatcher(std::basic_string<Char> const & seq)
    {
       for ( auto const & c : seq){
          this->push_back(std::make_unique<charMatcher<Char> >(c));
       }
    }
};

/*
 @brief match a string or the empty set
*/
template <std::equality_comparable Char>
struct optionalMatcher : exprMatcher<Char>{

    template <typename T>
    requires std::derived_from<T,exprMatcher<Char> >
    optionalMatcher(std::unique_ptr<T> && p)
    : matcher(std::move(p)){}
    void reset()override
    { this->matcher->reset();}

   [[nodiscard]] matchState consume(std::string & strIn, std::string & lexemeOut) override
   {
       if (!strIn.empty()){
          for (;;){
             switch (auto const result = matcher->consume(strIn,lexemeOut)){
                case matchState::Matched:
                   return result;
                case matchState::Eof:
                   [[fallthrough]];
                case matchState::MatchedEmpty:
                   [[fallthrough]];
                case matchState::NotMatched:
                   strIn += std::move(lexemeOut);
                   return matchState::MatchedEmpty;
                default:
                  assert(false);
                  return result;
             }
         }
      }else{
         return matchState::Eof;
      }
   }
    private:
    std::unique_ptr<exprMatcher<Char> > matcher;
};
// parallel matcher
template <std::equality_comparable Char>
struct orExprMatcher : exprMatcher<Char>{

   //submatcher
   struct matchInfo{
     matchInfo(std::unique_ptr<exprMatcher<Char> > && p)
     :matcher{std::move(p)},finished{false}{}
     void reset()
     {
        matcher->reset();
        finished = false;
     }
     std::unique_ptr<exprMatcher<Char> > matcher;
     bool finished = false; // set when this matcher is done
  };

   template <typename T>
      requires std::derived_from<T,exprMatcher<Char> >
   void push_back(std::unique_ptr<T> && p)
   {
      m_matchSeq.push_back(matchInfo(std::move(p)));
   }

   void reset() override
   {
      for ( auto & m : m_matchSeq){
         m.reset();
      }
   }

   [[nodiscard]] matchState consume(std::string & strIn, std::string & lexemeOut) override
   {
      if (! strIn.empty()){
         std::size_t matcherIdx = 0U;
         bool matchedEmpty = false;
         for ( auto & m : m_matchSeq){
            ++matcherIdx;
            if (! m.finished){
               switch(m.matcher->consume(strIn,lexemeOut)){
                  case matchState::MatchedEmpty:
                     matchedEmpty = true;
                     [[fallthrough]];
                  case matchState::Eof:
                     [[fallthrough]];
                  case matchState::NotMatched:
                     m.finished = true;  // this matcher is done
                     // is this the last matcher in the sequence?
                     if (matcherIdx == m_matchSeq.size()){
                        this->reset(); // always reset before return
                        if (matchedEmpty == true){  // matchedEmpty reuires retoring original string
                           // in fact should have been done by submatcher?
                           strIn = std::move(lexemeOut) + strIn; //backout
                           return matchState::MatchedEmpty;
                        }
                        else {
                           return matchState::NotMatched;
                        }
                     }else{ // not last matcher
                        // This matcher didnt match so restore the string state for the next matcher
                        strIn = std::move(lexemeOut) + strIn; //backout
                        break;
                     }
                  case matchState::Matched:
                     this->reset(); // always reset before return
                     return matchState::Matched;
                  default:
                     assert(false);
               }
            }
         }
         assert(false);
         return matchState::NotMatched;
      }else{
         return matchState::Eof;
      }
   }

  private:
    std::vector<matchInfo > m_matchSeq;

};
/**
@brief match any char
**/
template <std::equality_comparable Char>
struct any_matcher : primMatcher<Char>{
   bool operator()(Char const & ch) const override
   { return true;};
};

/**
@brief match one of a list of chars
**/
template <std::equality_comparable Char>
struct primOneOfMatcher : primMatcher<Char>{
   template <std::ranges::forward_range C>
   primOneOfMatcher(C const & cont){
      for ( auto c: cont){
        symbols.push_back(c);
      }
   }
   bool operator()(Char const & ch) const override
   {
      return std::ranges::find(symbols,ch) != symbols.end();
   }
   private:
   std::basic_string<Char> symbols;
};

/**
@brief match one from contiguous range of chars
**/
template <std::equality_comparable Char>
struct range_matcher : primMatcher<Char>{

     range_matcher(Char const & lowest, Char const & highest)
     : lowest{lowest},highest{highest}{
        assert( lowest <= highest);
     }

     bool operator()(Char const & ch) const override
     {
        return (ch >= lowest) && (ch <= highest);
     }
    private:
    Char const lowest;
    Char const highest;
};
