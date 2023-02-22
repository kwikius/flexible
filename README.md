Flexible
--------

 An easy to read and understand lexer expression language.

[traditionally regex syntax is highly cryptic.]
(https://stackoverflow.com/questions/12643009/regular-expression-for-floating-point-numbers)

Example traditional pattern for a floating point number:

```
[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
```

Example of code to lex a real number in Flex. Flex improves on the traditional form by
allowing to assign expressions to a variable

```
	/**
	 * @brief a lexer for a floating point number. 
	 **/
EXP               [eE]
SIGN              [+-]
DIGIT             [0-9]
EXPONENT          {EXP}{SIGN}?{DIGIT}+
REAL              {SIGN}?{DIGIT}*"."{DIGIT}+{EXPONENT}?|
{DIGIT}+(("."{EXPONENT}?)|{EXPONENT})
```

Same example in flexible. Flexible also allows assigning expressions to a variable.
Flexible (like flex) also adopts C and C++ style comments and semi-colon statement terminators.

```
/**
 * @brief a lexer for a floating point number. 
 **/
  exp       =  'eE'                                   ;
  sign      =  '+-'                                   ;
  digit     =  '0'~'9'                                ;                
  exponent  =  exp sign? digit+                       ;
  real      =  sign? digit* '.' digit+ exponent?
               | digit+ ( '.' exponent? | exponent )  ;
```

Flexible basic concepts
-----------------------
Characters and strings live in separate namespace as in C and C++.
This allows to use whitespace to format the code without affecting the pattern
and reduces the number of specila cases that need to be escaped. 
Only \" and ' need to be escaped ( \\' \\")
The result is that the lnaguage is much easier and faster to learn and the source code
is pleasant to read and easy to understand.

Single character literal
------------------------
A Single character is exppressed by surroundng with sinle quotes as in many programming languages

```singlechar = 'a' ;```

Alternate characters literal
---------------------------
Single quotes around a character sequence represents alternation

```
alternatives = 'abcxyz';
alternatives1 = 'a' | 'b' | 'c' | 'x' | 'y' | 'z' ; // alternate form
```

concatenation
-------------
Expressions are concatenated simply by sequencing with whitespace

```
concat = 'a' 'b' 'c' ; // represents the string "abc"
```

character strings
----------------
double quotes around a character sequence represents a more concise way to concatenate characters

```
str = "Hello World";
str1 = 'H' 'e' 'l' 'l' 'o' ' ' 'W' 'o' 'r' 'l' 'd' ; // alternate form
```
alternation 
-----------

altenatives are separated by | 

```
england  = "hello"      ;
france   = "salut"      ;
spain    = "hola"       ;
japan    = "Konnichiwa" ;

greeting = england | france | spain | japan ;
```

Empty set
---------

```empty_set = {};```

Empty set is only useful in connection with optional string. 
When the lexer matches the empty set, the current token is not consumed. 
Note matching Empty set is a particular semantic, this is not the same as "no match".  
Empty set on its own will cause an endless loop

Optional string
---------------
Optional string is described by postscript ?

```
very = ("very"' '+)  ;
niceness  = very? "nice" ;
optional1 = "abcd" | {} ; // alternative form
```

[Regular expressions in formal language theory]
(https://en.wikipedia.org/wiki/Regular_expression#Formal_language_theory)

One or more string
------------------

...etc






