
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
   NotMatched,  // ch is not a match so expression not matched
   Matching,  // ch is a match for last matched, but needs more to complete "matching"
   Matched,  // ch is a match and all elements matched "matched"
   MatchedEmpty // matched the empty set ( so don't consume ch)
};

/**
abstract base class expression matcher
**/
template <std::equality_comparable Char>
struct exprMatcher{
   virtual ~exprMatcher() = default;
   virtual void reset() = 0;
   virtual bool isMatching() const =0;
   [[nodiscard]] virtual matchState consume(Char const & ch) = 0;
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
   bool isMatching() const override { return false;} // no state
   matchState consume(Char const & ch) override
   {
      return (*this)(ch)
         ? matchState::Matched
         : matchState::NotMatched
      ;
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
**/
template <std::equality_comparable Char>
struct emptyMatcher : exprMatcher<Char>{
    void reset() override{}
    bool isMatching() const override { return false;}
    matchState consume(Char const & ch) override
    {
       return matchState::MatchedEmpty;
    }
};

/**
 @brief match a sequence of exprMatchers ( technically a string, but general)
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

   bool isMatching() const override
   { return currentMatchIndex > 0;}

   [[nodiscard]] matchState consume(Char const & ch) override
   {
      if ( this->m_matchSeq.empty()){
         return matchState::NotMatched;
      }
      for (;;){
         auto const result = this->m_matchSeq[currentMatchIndex]->consume(ch);
         switch(result){
            case matchState::MatchedEmpty:
               if (++currentMatchIndex < std::ssize(m_matchSeq)){
                  break; // dont consume char go round again
               }else{
                  this->reset();
                  return matchState::MatchedEmpty;
               }
            case matchState::Matched:
               if (++currentMatchIndex < std::ssize(m_matchSeq)){
                  return matchState::Matching;
               }else{
                  this->reset();
                  return matchState::Matched;
               }
            case matchState::NotMatched:
               this->reset();
               return matchState::NotMatched;
            case matchState::Matching:
               return matchState::Matching;
            default:
               assert(false);
         }
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
    bool isMatching() const
    { return this->matcher->isMatching();}
   [[nodiscard]] matchState consume(Char const & ch) override
   {
       switch (auto const result = matcher->consume(ch)){
          case matchState::Matched:
             [[fallthrough]];
          case matchState::Matching:
             return result;
          case matchState::MatchedEmpty:
             [[fallthrough]];
          case matchState::NotMatched:
             return matchState::MatchedEmpty;
          default:
            assert(false);
            return result;
       }
   }
    private:
    std::unique_ptr<exprMatcher<Char> > matcher;
};
// parallel matcher
template <std::equality_comparable Char>
struct orExprMatcher : exprMatcher<Char>{

   struct matchInfo{
     matchInfo(std::unique_ptr<exprMatcher<Char> > && p)
     :matcher{std::move(p)},finished{false}{}
     void reset()
     {
        matcher->reset();
        finished = false;
     }
     bool isMatching() const
     {
        return matcher->isMatching();
     }
     std::unique_ptr<exprMatcher<Char> > matcher;
     bool finished = false;
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

   bool isMatching() const override
   {
      for ( auto const & m : m_matchSeq){
         if ( m.isMatching()) {
            return true;
         }
      }
      return false;
   }

   [[nodiscard]] matchState consume(Char const & ch) override
   {
      // set to true if one or more matchers is matching
      bool matching = false;
      // set to true if one or more matchers matched empty
      bool matchedEmpty = false;
      for ( auto & m : m_matchSeq){
         if (! m.finished){
            switch(m.matcher->consume(ch)){
               case matchState::MatchedEmpty:
                 m.finished = true;
                 matchedEmpty = true;
                 break;
               case matchState::Matched:
                 this->reset();
                 return matchState::Matched;
               case matchState::Matching:
                 matching = true;  // still matching in progress
                 break;
               case matchState::NotMatched:
                 m.finished = true; // this matcher is done
                 break;
               default:
               assert(false);
            }
         }
      }
      if ( matching){
         return matchState::Matching;
      }else{
         // no matchers in progress
         this->reset();
         if (matchedEmpty){  // one or more matcher matched empty
            return matchState::MatchedEmpty;
         }else{
            return matchState::NotMatched;
         }
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

void any_test()
{
   std::cout << "any_test\n";

   std::string const str1 = "absdefghijkmnopqrstuvwxyz"
   "1234567890!\"£$%^&*()_+=-QWERTYUIOP{}[]ASDFGHJKL:@~;'#"
   "|\\ZXCVBNM<,>.?/";
   any_matcher<std::string::value_type> const match;

   for (auto c : str1){
      if (! match(c)){
         QUAN_CHECK(false);
      }
   }
}

void one_of_list_test()
{
   std::cout << "one_of_list_test()\n";

   std::string const str = "abc";
   primOneOfMatcher<std::string::value_type> match = str;

   for ( auto c : str){
      QUAN_CHECK( match(c));
   }

   std::string const str1 = "xyz";

   for ( auto c : str1){
      QUAN_CHECK(match(c) == false);
   }

   std::string str2  = "a123";

   QUAN_CHECK(match(str2[0])==true)
   QUAN_CHECK(match(str2[1])==false)
}

void range_test()
{
   range_matcher<std::string::value_type> const match{'b','e'};

   QUAN_CHECK( match('b') == true)
   QUAN_CHECK( match('a') == false)
   QUAN_CHECK( match('e') == true)
   QUAN_CHECK( match('f') == false)
}

void match_expr_test()
{
   auto me = matchSequence<char>{};
   QUAN_CHECK( me.isMatching() == false);
   // me = 'ab';
   me.push_back(std::make_unique<primOneOfMatcher<char> >("ab"));
   QUAN_CHECK( me.isMatching() == false);

   QUAN_CHECK(me.consume('a') == matchState::Matched);
   QUAN_CHECK(me.isMatching() == false);
   QUAN_CHECK(me.consume('x') == matchState::NotMatched);
   QUAN_CHECK( me.isMatching() == false);

   // me = 'ab' 'de' ;
   me.push_back(std::make_unique<primOneOfMatcher<char> >("de"));
   QUAN_CHECK(me.isMatching() == false);
   // match "ae"
   QUAN_CHECK(me.consume('a') == matchState::Matching);
   QUAN_CHECK(me.isMatching() == true);
   QUAN_CHECK(me.consume('e') == matchState::Matched);
   QUAN_CHECK(me.isMatching() == false);
   // match "aa"
   QUAN_CHECK(me.consume('a') == matchState::Matching);
   QUAN_CHECK(me.isMatching() == true);
   QUAN_CHECK(me.consume('a') == matchState::NotMatched);
   QUAN_CHECK(me.isMatching() == false);
}

void or_matcher_test()
{
   auto m = orExprMatcher<char>{};
   // m = "abc" | "de" ;
   m.push_back(std::make_unique<simpleStringMatcher<char> >("abc"));
   m.push_back(std::make_unique<simpleStringMatcher<char> >("de"));
   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('a') == matchState::Matching)
   QUAN_CHECK(m.isMatching() == true)
   QUAN_CHECK(m.consume('b') == matchState::Matching)
   QUAN_CHECK(m.isMatching() == true)
   QUAN_CHECK(m.consume('c') == matchState::Matched)

   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('d') == matchState::Matching)
   QUAN_CHECK(m.isMatching() == true)
   QUAN_CHECK(m.consume('e') == matchState::Matched)

   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('c') == matchState::NotMatched)

   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('a') == matchState::Matching)
   QUAN_CHECK(m.consume('b') == matchState::Matching)
   QUAN_CHECK(m.consume('b') == matchState::NotMatched)
}

void optional_matcher_test()
{
   auto m = optionalMatcher<char>{std::make_unique<simpleStringMatcher<char> >("ab")};
   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('a') == matchState::Matching)
   QUAN_CHECK(m.isMatching() == true)
   QUAN_CHECK(m.consume('b') == matchState::Matched)

   QUAN_CHECK(m.isMatching() == false)
   QUAN_CHECK(m.consume('a') == matchState::Matching)
   QUAN_CHECK(m.isMatching() == true)
   QUAN_CHECK(m.consume('x') == matchState::MatchedEmpty)
   QUAN_CHECK(m.isMatching() == false)

   auto m1 = optionalMatcher<char>{std::make_unique<primOneOfMatcher<char> >("ab")};

   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('a') == matchState::Matched)
   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('b') == matchState::Matched)

   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('x') == matchState::MatchedEmpty)

}

int errors = 0;
int main()
{
  any_test();
  one_of_list_test();
  range_test();

  match_expr_test();
  or_matcher_test();

  optional_matcher_test();

  EPILOGUE;
}
