Project made at UMSL. It is a global scope parser. Language Definions are below.
```text
/*
 * Language BNF
 * 
 * <program>    -> program <vars> <block>
 * <vars>       -> empty | var <varList>
 * <varList>    -> identifier , integer <varlist2>
 * <varlist2>   -> ; | <varList>
 * <stats>      -> <stat> <mStat>
 * <mStat>      -> empty | <stat> <mStat>
 * <stat>       -> <read> | <print> | <block> | <cond> | <iter> | <assign>
 * <block>      -> start <vars> <stats> stop
 * <read>       -> read identifier ;
 * <print>      -> print <exp> ;
 * <cond>       -> iff [ <exp> <relational> <exp> ] <stat>
 * <iter>       -> iterate [ <exp> <relational> <exp> ] <stat>
 * <assign>     -> set identifier <exp> ;
 * <relational> -> .le. | .ge. | .lt. | .gt. | ** | ~
 * <exp>        -> <M> <exp2>
 * <exp2>       -> + <exp> | - <exp> | empty
 * <M>          -> <N> <M2>
 * <M2>         -> % <M> | empty
 * <N>          -> <R> <N2> | - <N>
 * <N2>         -> / <N> | empty
 * <R>          -> ( <exp> ) | identifier | integer
 */
```
 
 /*
Lexical Definitions
  * Case sensitive
  * Each scanner error should display "LEXICAL ERROR: " followed by details including the line number if counted
  * Alphabet
    + all English letters (upper and lower), digits, plus the extra characters as seen below, plus white spaces
    + No other characters allowed and they should generate errors
  * Identifiers
    + begin with any letter
    + continue with any number of letters and digits and underscores, up to 8 significant total
      - you may assume no identifier is longer than 8 characters (in testing, this is intended to make it easier not harder)
  * Keywords (reserved)
    + start stop iterate var exit read print iff then set func program
  * Operators and delimiters (single or double character, can produce individual tokens or as a group with instances)
    + =  .le.  .ge. .lt. .gt. ~ : ;  +  -  **  /  %   (  ) , { } ; [ ]
    + Note the mult-char tokens, without spaces
    + Note: '.' removed 10/9/24, if you have it already it is fine
  * Integers
    + any sequence of decimal digits, no sign, no decimal point, up to 8 significant
    + you may assume no number longer than 8 characters (in testing)
  * Comments start with @@ and end with @ and contain any characters
    + you may assume no white spaces inside as in @@thisiscomment@ especially if implementing option 1
    + you may also assume no \n inside
*/

/*
Static Semantics Definition
  * We impose rules on proper definition and use of variables
    + Variable definition
       - any identifier listed in any <varList> is definition of the variable named as the identifier instance
    + Variable use - identifier showing up in any statement (not under <varList>)
  * Static semantic rules
    + Variable can only be defined once in a scope, a second definition is an error, should be reported and the program exits
    + Variable must be defined before its first use, otherwise the variable is reported as an error and the program exits
    + A defined variable must be used, otherwise report a detailed warning and continue the program
*/
