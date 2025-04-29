#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <map>
#include <unordered_set>

#define STATES 11   //How many states in the FSA
#define COLCHARS 26 //How many specific char cases there are for the FSA

//Defines a token
struct Token {
  std::string tokenId;  //What type of token this is
  std::string instance; //What is the token actually
  int line;             //What line the token is on
  
  Token(const std::string& tokenId, const std::string& instance, int line);
  Token();
};

//A map that relates passed characters to a specific column in the STATE_TABLE
extern const std::map<char, int> COLMAP;

//A map holding all the error messages depending on the error state
extern const std::map<int, std::string> ERRORS;

extern const int STATE_TABLE[STATES][COLCHARS]; //A state table relating to the DFSA for this language

const int KEYWORDSIZE = 12; //How many keywords are in the language
//An unordered_set holding the string representation of every keyword
extern const std::unordered_set<std::string> KEYWORDS;

//A map that relates a final state to a specific token
extern const std::map<int, std::string> TOKENS;

#endif