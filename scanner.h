#ifndef SCANNER_H
#define SCANNER_H

#include "language.h"


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
 *  Description: Builds a single token from a input filestream every time it is called. 
 *               Navigates the DFSA described in STATE_TABLE in language.h.
 *               Returns EOF_tk at the end of input filestream.
 *               Throws a invalid argument error when a non valid token is found.
 *  Passed: A file stream pointing to the input file
 *  Return: Returns a token based on the input from the filestream.
 */
Token scanner(std::ifstream &scannerIn);

#endif