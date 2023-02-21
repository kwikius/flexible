
#include <quan_matters/test/test.hpp>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <list>
#include <cassert>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <quan/meta/is_element_of.hpp>

enum class matchState{
   NotMatched,  // ch is not a match so expression not matched
   Matching,  // ch is a match for last matched, but needs more to complete "matching"
   Matched  // ch is a match and all elements matched "matched"
};


/**
abc expression matcher
**/
template <std::equality_comparable Char>
struct expr_matcher{
   virtual ~expr_matcher() = default;
   virtual void reset() = 0;
   virtual bool isMatching() const =0;
   [[nodiscard]] virtual matchState consume(Char const & ch) = 0;
};

/**
@brief abstract base class single character matcher
**/
template <std::equality_comparable Char>
struct prim_matcher{
   virtual ~prim_matcher() = default;
   /**
   @brief return true if character was matched, else false
   **/
   virtual bool operator()(Char const & ch) const = 0;
};

/*
  holds a sequence of prim_matchers/ expr_matchers
*/
template <std::equality_comparable Char>
struct matchSequence : expr_matcher<Char>{
   matchSequence() : currentMatchIndex{0}{}

   using Variant = std::variant<
      std::unique_ptr<prim_matcher<Char> >,
      std::unique_ptr<expr_matcher<Char> >
   >;
   template <typename T>
      requires (std::derived_from<T,prim_matcher<Char> > || std::derived_from<T,expr_matcher<Char> >)
   void push_back(std::unique_ptr<T> && p)
   {
      m_matchSeq.push_back(std::move(p));
   }
   void reset() override
   {
      for ( auto & m : m_matchSeq){
         if (std::holds_alternative< std::unique_ptr<expr_matcher<Char> > > (m)){
            std::get< std::unique_ptr<expr_matcher<Char> > > (m)->reset();
         }
      }
      currentMatchIndex = 0;
   }
    //https://open-std.org/JTC1/SC22/WG21/docs/papers/2018/p0051r3.pdf
   template <typename... F>
   struct overload : F... { using F::operator()...;};

   bool isMatching() const override
   { return currentMatchIndex > 0;}
   [[nodiscard]] matchState consume(Char const & ch) override
   {
      if ( this->m_matchSeq.empty()){
         return matchState::NotMatched;
      }
      auto & childMatcher = this->m_matchSeq[currentMatchIndex];
      auto const result = std::visit (
         overload {
            [this,ch] (std::unique_ptr<expr_matcher<Char> > & m) -> matchState
               { return m->consume(ch);},
            [this,ch]( std::unique_ptr<prim_matcher<Char> > & m) -> matchState
               { return ((*m)(ch)) ? matchState::Matched : matchState::NotMatched;}
         },
         childMatcher
      );

      switch(result){
         case matchState::Matched:
            if (++currentMatchIndex < std::ssize(m_matchSeq)){
               return matchState::Matching;
            }
            [[fallthrough]];
         case matchState::NotMatched:
            this->reset();
            [[fallthrough]];
         case matchState::Matching:
            break;
         default:
            assert(false);
      }
      return result;
   }
   private:
   std::vector<Variant> m_matchSeq;
   int currentMatchIndex;
};

/**
@brief match any char
**/
template <std::equality_comparable Char>
struct any_matcher : prim_matcher<Char>{
   bool operator()(Char const & ch) const override
   { return true;};
};

/**
@brief match one of list of chars
**/
template <std::equality_comparable Char>
struct one_of_list_matcher : prim_matcher<Char>{
   template <std::ranges::forward_range C>
   one_of_list_matcher(C const & cont){
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
struct range_matcher : prim_matcher<Char>{

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

   std::string str = "abc";
   one_of_list_matcher<std::string::value_type> match = str;

   for ( auto c : str){
      QUAN_CHECK( match(c));
   }

   std::string str1 = "xyz";

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
   me.push_back(std::make_unique<one_of_list_matcher<char> >("ab"));
   QUAN_CHECK( me.isMatching() == false);

   QUAN_CHECK(me.consume('a') == matchState::Matched);
   QUAN_CHECK(me.isMatching() == false);
   QUAN_CHECK(me.consume('x') == matchState::NotMatched);
   QUAN_CHECK( me.isMatching() == false);

   // me = 'ab' 'de' ;
   me.push_back(std::make_unique<one_of_list_matcher<char> >("de"));
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

int errors = 0;
int main()
{
  any_test();
  one_of_list_test();
  range_test();

  match_expr_test();

  EPILOGUE;
}
