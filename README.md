***Flexible***

 A lexer expression language

[traditionally regex syntax is highly cryptic.]
(https://stackoverflow.com/questions/12643009/regular-expression-for-floating-point-numbers)

Example traditional pattern for a floating point number:

```
[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
```

Example in flexible

```
  exp       =  'eE'                ;
  sign      =  '+-'                ;
  digit     =  '0'..'9'            ;                
  exponent  =  exp sign? digit+    ;
  real      =  sign? digit* '.' digit+ exponent?
               | digit+ ( '.' exponent? | exponent )  ;

```

[Regular expressions are defined in formal language theory]
(https://en.wikipedia.org/wiki/Regular_expression#Formal_language_theory)




