#include <fstream>
#include <iostream>
#include <cctype>
#include <iomanip>

#include "language.h"
#include "scanner.h"

static char filter(std::ifstream &scannerIn, int &currentCol, int &lookAheadCol);
static char skipComments(std::ifstream &scannerIn);
static int lookAhead(const int CURRENTSTATE, const int LOOKAHEADCOL);
static void handleError(const int ERRORSTATE, const int LINE);
static std::string checkKeyword(const std::string INSTANCE);

/*
 *  Description: Builds a single token from a input filestream every time it is called. 
 *               Navigates the DFSA described in STATE_TABLE in language.h.
 *               Returns EOF_tk at the end of input filestream.
 *               Throws a invalid argument error when a non valid token is found.
 *  Passed: A file stream pointing to the input file
 *  Return: Returns a token based on the input from the filestream.
 */
Token scanner(std::ifstream &scannerIn) 
{
  int currentState = 0;
  static int line = 1;
  std::string tokenState = "";
  
  while(currentState < 100)
  {
    int currentCol;
    int lookAheadCol;
    
    char currentChar = filter(scannerIn, currentCol, lookAheadCol);
    
    if(currentChar == '\xff') handleError(1003, line); //char not in language found in filestream
    if(currentChar == '`') handleError(1004, line); //Invalid comment found by filter
    
    if(currentChar == '\0') return Token("EOF_tk", "EOF", line); //Filter returns '\0' for EOF
    
    if(currentChar == '\n') line++; //Count Line numbers
    if(!isspace(currentChar)) tokenState += currentChar; //Don't append whitespaces
    
    currentState = STATE_TABLE[currentState][currentCol];
    if(currentState >= 1000) handleError(currentState, line); //Error
    if(currentState >= 100) //Final State
    {
      std::string tokenName = TOKENS.at(currentState); //Get token name
      
      return Token(tokenName, tokenState, line);
    }
    else
    {
      int lookAheadEnd = lookAhead(currentState, lookAheadCol);
      if(lookAheadEnd >= 1000) handleError(lookAheadEnd, line); //Error   
      if(lookAheadEnd >= 100) //Only used by ID_tk NUM_tk
      {
        std::string tokenName = TOKENS.at(lookAheadEnd); //Get token name
        if(tokenName == "ID_tk") tokenName = checkKeyword(tokenState);
        
        return Token(tokenName, tokenState, line);
      }
    }
  }
  return Token("", "", 0);
}

/*
 *  Description: This function reads a char from the filestream skipping comments. Comments are @@word@ 
 *               If any chars not in language are found a '\xff' is returned.
 *  Passed: A filestream point to the input file, Integer relating to the currentCol in STATE_TABLE.
 *          A integer relating to the next characters column in the STATE_TABLE
 *  Returns: Char that is the next char in the file stream. The currentCol is the STATE_TABLE column of 
 *           the returned char. LookAheadCol is set as the column of the next chacter in the filestream.
 *           If skipComment helper returns ` invalid comment was found. Pass the ` back to the scanner.
 */
static char filter(std::ifstream &scannerIn, int &currentCol, int &lookAheadCol) 
{
  char currentChar;
  scannerIn.get(currentChar);
  
  //Skip comments
  if(currentChar == '@') currentChar = skipComments(scannerIn);
  if(currentChar == '`') return '`'; //Invalid comment
  
  char lookAhead = scannerIn.peek();
  
  if(scannerIn.eof()) return '\0';
  
  try
  {
    currentCol = COLMAP.at(currentChar);
    lookAheadCol = COLMAP.at(lookAhead);
  } 
  catch(const std::out_of_range &e)
  {
    return '\xff';
  }
  
  return currentChar;
}

/*
 *  Description: Skips comments in the form of @@words@
 *  Passed: The input filestream.
 *  Return: Returns the first char after the skipped comment. If the comment has the incorrrect form it will
 *          return `.
 */
static char skipComments(std::ifstream &scannerIn)
{
  char skipChar;
  scannerIn.get(skipChar);
  
  //Requires a double @@ at the start of a comment
  if(skipChar != '@') return '`';//throw std::invalid_argument("LEXICAL ERROR: Invalid Comment");
    
  do //Skip until end of comment
  {
    scannerIn.get(skipChar);
    if(scannerIn.eof()) return '`'; //throw std::invalid_argument("LEXICAL ERROR: Invalid Comment");
  } while(skipChar != '@');
  
  scannerIn.get(skipChar);
  return skipChar;
}

/*
 *  Description: This function determines if the next character in the stream would denote a end of state
 *               for the current toekn. This is used by ID_tk and NUM_tk.
 *  Passed: This function is passed what the currentState is in the DFSA. 
 *          Also passed what column the next character is in the STATE_TABLE.
 *  Return: Returns what the next state is. Will return a -1 whenever it is not in state 9 for ID_tk or 10
 *          for NUM_tk.
 */
static int lookAhead(const int CURRENTSTATE, const int LOOKAHEADCOL)
{
  //Only INT_tk and ID_tk care about lookahead
  if(CURRENTSTATE == 9 || CURRENTSTATE == 10)
  {
    return STATE_TABLE[CURRENTSTATE][LOOKAHEADCOL];
  }
  
  return -1;
}

/*
 *  Description: Takes a given error state and checks its value in the ERRORS map. 
 *               Throws a invalid argument of the corresponding error value.
 *  Passed: Passed the ERRORSTATE from the STATE_TABLE and the current line number.
 *  Return: Returns nothing throwns a invalid_argument error.
 */
static void handleError(const int ERRORSTATE, const int LINE) 
{
  std::string error = ERRORS.at(ERRORSTATE);
  error += std::to_string(LINE);
  throw std::invalid_argument(error);
}

/*
 *  Description: Checks if a given ID_tk state is a KEYWORD or not.
 *  Passed: Passed the current instance of the id.
 *  Returns: KEYWORD_tk if the instance is in the KEYWORD map.
 */
static std::string checkKeyword(const std::string INSTANCE) 
{
  if (KEYWORDS.find(INSTANCE) != KEYWORDS.end()) return "KEYWORD_tk";
  
  return "ID_tk";
}
