
#include <flexible/matchers/regexMatcher.hpp>

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

   auto m1 = optionalMatcher<char>{std::make_unique<primOneOfMatcher<char> >("ab")};

   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('a') == matchState::Matched)
   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('b') == matchState::Matched)

   QUAN_CHECK(m1.isMatching() == false)
   QUAN_CHECK(m1.consume('x') == matchState::MatchedEmpty)

   // make optional with or matcher  m = "abc" | {} ;
   // check is same semantic
   auto m2 = orExprMatcher<char>{};
   m2.push_back(std::make_unique<simpleStringMatcher<char> >("abc"));
   m2.push_back(std::make_unique<emptyMatcher<char> > ());

   QUAN_CHECK(m2.isMatching() == false)
   QUAN_CHECK(m2.consume('a') == matchState::Matching)
   QUAN_CHECK(m2.isMatching() == true)
   QUAN_CHECK(m2.consume('b') == matchState::Matching)
   QUAN_CHECK(m2.isMatching() == true)
   QUAN_CHECK(m2.consume('c') == matchState::Matched)

   QUAN_CHECK(m2.isMatching() == false)

   QUAN_CHECK(m2.consume('a') == matchState::Matching)
   QUAN_CHECK(m2.isMatching() == true)
   QUAN_CHECK(m2.consume('b') == matchState::Matching)
   QUAN_CHECK(m2.isMatching() == true)
   QUAN_CHECK(m2.consume('d') == matchState::MatchedEmpty)
   QUAN_CHECK(m2.isMatching() == false)

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
