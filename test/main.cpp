
#include <flexible/matchers/regexMatcher.hpp>
#include <quan_matters/test/test.hpp>

const char * getMatchStr(matchState state)
{
   switch(state){

      case matchState::Matched:
         return "matchState::Matched";
      case matchState::MatchedEmpty:
         return "matchState::MatchedEmpty";
      case matchState::NotMatched:
         return "matchState::NotMatched";
      case matchState::Eof:
         return "matchState::Eof";
      default:
         return "Unknown MatchState";
   }
}

void any_test()
{
   std::cout << "any_test\n";

   std::string const str1 = "absdefghijkmnopqrstuvwxyz"
   "1234567890!\"£$%^&*()_+=-QWERTYUIOP{}[]ASDFGHJKL:@~;'#"
   "|\\ZXCVBNM<,>.?/";
   charAnyMatcher<std::string::value_type> const match;

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
   charAlternateMatcher<std::string::value_type> match = str;

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
   std::cout << "range_test()\n";
   charRangeMatcher<std::string::value_type> const match{'b','e'};

   QUAN_CHECK( match('b') == true)
   QUAN_CHECK( match('a') == false)
   QUAN_CHECK( match('e') == true)
   QUAN_CHECK( match('f') == false)
}

void match_expr_test()
{
   std::cout << "match_expr_test()\n";
   auto me = exprConcatMatcher<char>{};

   // flexible syntax
   // me = 'ab';
   me.push_back(std::make_unique<charAlternateMatcher<char> >("ab"));

   std::string lexeme;
   std::string str = "a";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(lexeme == "a")

   QUAN_CHECK(str == "")
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Eof)
   QUAN_CHECK(lexeme == "a")

   lexeme = "";  // reset lexeme;
   str = "x";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::NotMatched)
   QUAN_CHECK(lexeme == "x") // lexeme holds partial match inc failed last char
   QUAN_CHECK(str == "")

   // flexible syntax
   // me = 'ab' 'de' ;
   me.push_back(std::make_unique<charAlternateMatcher<char> >("de"));

   str = "ad";
   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(str == "")
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Eof)
   QUAN_CHECK(lexeme == "ad")

   str = "bd";
   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(str == "")
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Eof)
   QUAN_CHECK(lexeme == "bd")

   str = "ae";
   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(str == "")
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Eof)
   QUAN_CHECK(lexeme == "ae")

   str = "be";
   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(str == "")
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Eof)
   QUAN_CHECK(lexeme == "be")

   str = "adx";
   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::Matched);
   QUAN_CHECK(str == "x")
   QUAN_CHECK(lexeme == "ad")

   lexeme = "";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::NotMatched)
   QUAN_CHECK(lexeme == "x")
   QUAN_CHECK(str=="")

   lexeme = "";
   str = "aa";
   QUAN_CHECK(me.consume(str,lexeme) == matchState::NotMatched)
   QUAN_CHECK(lexeme == "aa")
   QUAN_CHECK(str == "")
}

void or_matcher_test()
{
   std::cout << "or_matcher_test()\n";

   std::string lexeme;
   std::string str ;

   auto m1 = exprAlternateMatcher<char>{};
   // m1 = "abc" | "de" ;
   m1.push_back(std::make_unique<charConcatMatcher<char> >("abc"));
   m1.push_back(std::make_unique<charConcatMatcher<char> >("de"));

   str = "abc";
   lexeme = "";
   QUAN_CHECK(m1.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(lexeme == "abc")
   QUAN_CHECK(str == "")

   str = "de";
   lexeme = "";
   auto const s = m1.consume(str,lexeme);
   QUAN_CHECK( s == matchState::Matched)
   QUAN_CHECK(lexeme == "de")
   QUAN_CHECK(str=="")

   auto m2 = exprAlternateMatcher<char>{};

   // check that order of alternatives doesnt make a difference.
   // m2 = "de" | "abc" ;
   m2.push_back(std::make_unique<charConcatMatcher<char> >("de"));
   m2.push_back(std::make_unique<charConcatMatcher<char> >("abc"));

   str = "abc";
   lexeme = "";
   QUAN_CHECK(m2.consume(str,lexeme) == matchState::Matched)
   QUAN_CHECK(lexeme == "abc")
   QUAN_CHECK(str == "")

   str = "de";
   lexeme = "";
   auto const s2 = m2.consume(str,lexeme);
   QUAN_CHECK( s2 == matchState::Matched)
   QUAN_CHECK(lexeme == "de")
   QUAN_CHECK(str == "")
}

void optional_matcher_test()
{
   std::cout << "optional_matcher_test()\n";
   auto m1 = exprOptionalMatcher<char>{std::make_unique<charConcatMatcher<char> >("hello")};

   std::string str;
   std::string lexeme;

   // valid string
   // Output mathed string is in lexeme and string is empty
   // returns matchState::Matched
   str = "hello";
   lexeme ="";
   auto const s1 = m1.consume(str,lexeme);
   QUAN_CHECK(s1 == matchState::Matched)
   QUAN_CHECK(lexeme == "hello")
   QUAN_CHECK(str == "")

   // invalid string returns MathcedEmpty and original string is intact
   str = "goodbye";
   lexeme = "";
   auto const s2 = m1.consume(str,lexeme);
   QUAN_CHECK(s2 == matchState::MatchedEmpty)
   QUAN_CHECK(lexeme == "")
   QUAN_CHECK(str == "goodbye")
}

/*
multiple lexemes in one string
*/
void longer_string_test()
{

}

/*
struct matcher{
   template <typename T>
   requires std::derived_from<T,exprMatcher<char> >
   matcher(std::unique_ptr<T> && m): m_matcher{std::move(m)}{}
   matchState match( char ch)
   {
      lexeme += ch;
      return m_matcher->consume(ch);
   }
   void backtrack( std::string & str)
   {
      str = std::move(this->lexeme) + str;
   }

   std::unique_ptr<exprMatcher<char> > m_matcher;
   std::string lexeme;
};

void matcher_test()
{
  auto make_matcher = [] (){
     auto m = std::make_unique<exprAlternateMatcher<char> >();
     m->push_back(std::make_unique<charConcatMatcher<char> >("abc"));
     m->push_back(std::make_unique<charConcatMatcher<char> >("wxyz"));
     m->push_back(std::make_unique<emptyMatcher<char> > ());
     return matcher{std::move(m)};
  };

  auto pop_front =[](std::string & str)-> char{
     auto ch = str.front();
     str.erase(0,1);
     return ch;
  };

  matcher m = make_matcher();
  std::string str = "wxyzabx";
  std::string lexeme;
  bool done = false;
  while(str.length() && ! done ){
     switch(m.match(pop_front(str))){
        case matchState::Matched:
           lexeme = std::move(m.lexeme);
           std::cout << "Matched. \"" << lexeme << "\"\n" ;
           break;
        case matchState::Matching:
           break;
        case matchState::NotMatched:
           // must be a fail so invalid
           std::cout << "Not Matched. parsed \""<< lexeme << "\" residual = \"" << str << "\"\n";
           done = true;
           break;
        case matchState::MatchedEmpty:
           m.backtrack(str);
           std::cout << "Empty Matched. residual = \"" << str << "\"\n";
           // since there are no more matchers then done
           done = true;
           break;
        default:
           break;
      }
   }
   if (! str.length() ){
      if ( m.lexeme.length()){
        std::cout << "EOF while matching = \"" << lexeme << "\"\n";
      }
   }


}
*/

int errors = 0;
int main()
{
  any_test();
  one_of_list_test();
  range_test();

  match_expr_test();
  or_matcher_test();
//
//  optional_matcher_test();

 // matcher_test();

  EPILOGUE;
}
