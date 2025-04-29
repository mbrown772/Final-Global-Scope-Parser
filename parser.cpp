#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "parser.h"
#include "language.h"
#include "scanner.h"
#include "tree.h"

/*
 * Object for the scanner information to be passed throughout the program.
 */
struct ScannerObj {
  std::ifstream scannerIn; //File stream in for the scanner
  Token scannerToken;      //Token the scanner just read in
};

static void getToken(ScannerObj &scannerObj);
static void handleError(const std::string EXPECTED, const std::string GIVEN, const int LINE);

static std::unique_ptr<Node> program(ScannerObj &scannerObj);
static std::unique_ptr<Node> vars(ScannerObj &scannerObj);
static std::unique_ptr<Node> block(ScannerObj &scannerObj);
static std::unique_ptr<Node> varlist(ScannerObj &scannerObj);
static std::unique_ptr<Node> varlist2(ScannerObj &scannerObj);

static std::unique_ptr<Node> stats(ScannerObj &scannerObj);
static std::unique_ptr<Node> mstat(ScannerObj &scannerObj);
static std::unique_ptr<Node> stat(ScannerObj &scannerObj);
static std::unique_ptr<Node> read(ScannerObj &scannerObj);
static std::unique_ptr<Node> print(ScannerObj &scannerObj);
static std::unique_ptr<Node> cond(ScannerObj &scannerObj);
static std::unique_ptr<Node> iter(ScannerObj &scannerObj);
static std::unique_ptr<Node> assign(ScannerObj &scannerObj);
static std::unique_ptr<Node> relational(ScannerObj &scannerObj);

static std::unique_ptr<Node> exp(ScannerObj &scannerObj);
static std::unique_ptr<Node> exp2(ScannerObj &scannerObj);
static std::unique_ptr<Node> M(ScannerObj &scannerObj);
static std::unique_ptr<Node> M2(ScannerObj &scannerObj);
static std::unique_ptr<Node> N(ScannerObj &scannerObj);
static std::unique_ptr<Node> N2(ScannerObj &scannerObj);
static std::unique_ptr<Node> R(ScannerObj &scannerObj);


/*
 * Auxiliary function for the parser. Opens the file passed by FILENAME
 * and calls the first nonterminal in the BNF. Catches any invalid_argument
 * errors thrown by the program. Returns NULL if a error was found when parsing
 * or returns the root of the parse tree.
 */
std::unique_ptr<Node> parser(const std::string FILENAME) 
{
  ScannerObj scannerObj; //Object to pass 
  scannerObj.scannerIn.open(FILENAME.c_str());
  if (!scannerObj.scannerIn.is_open()) 
  {
    throw std::invalid_argument("ERROR: scannerIn failed to open");
  }
  std::unique_ptr<Node> root = nullptr;

  try
  {
    getToken(scannerObj);
    root = program(scannerObj); //Call the first nonterminal in the BNF
  }
  catch(const std::invalid_argument &e) //Scanner or Parser found a error
  {
    std::cout << std::endl << e.what() << std::endl;
    return nullptr;
  }
  
  return root;
}


//Helper function print error messages and throw a invalid_argumnet excetption
//Passed what the parser expected to see, what it was actually given, and what line it was on.
static void handleError(const std::string EXPECTED, const std::string GIVEN, const int LINE)
{
  std::stringstream error; 
  error << "ERROR || Expected: " << EXPECTED << " || Given: " << GIVEN << " || Line: " << LINE;
  throw std::invalid_argument(error.str());
}

//Helper function to get the next token from the scanner and save in scannerObj
static void getToken(ScannerObj &scannerObj)
{
  scannerObj.scannerToken = scanner(scannerObj.scannerIn);
}

/* Function for the non-terminal program in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <program> -> program <vars> <block>
 */
static std::unique_ptr<Node> program(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode= new Node("program");
  std::unique_ptr<Node> returnNode(new Node("program"));

  if(scannerObj.scannerToken.tokenId != "KEYWORD_tk" && scannerObj.scannerToken.instance != "program")
    handleError("program", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  returnNode->child1 = vars(scannerObj);
  
  returnNode->child2 = block(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "EOF_tk") //Make sure all tokens out of the file are used
    handleError("EOF", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  
  return returnNode;
}


/* Function for the non-terminal vars in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <vars> -> empty | var <varList>
 */
static std::unique_ptr<Node> vars(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("vars");
  std::unique_ptr<Node> returnNode(new Node("vars"));
  
  if(scannerObj.scannerToken.tokenId == "KEYWORD_tk" && scannerObj.scannerToken.instance == "var")
  {
    getToken(scannerObj);
    
    returnNode->child1 = varlist(scannerObj);
    
    return returnNode;
  }

  return returnNode; //Empty
}

/* Function for the non-terminal varlist in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <varList> -> identifier , integer <varlist2>
 */
static std::unique_ptr<Node> varlist(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("varlist");
  std::unique_ptr<Node> returnNode(new Node("varlist"));
  
  if(scannerObj.scannerToken.tokenId != "ID_tk") 
    handleError("identifier", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "COMMA_tk") 
    handleError(",", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "INT_tk") 
    handleError("integer", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj);
  
  returnNode->child1 = varlist2(scannerObj);

  return returnNode;
}

/* Function for the non-terminal varlist2 in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <varlist2> -> ; | <varList>
 */
static std::unique_ptr<Node> varlist2(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("varlist2");
  std::unique_ptr<Node> returnNode(new Node("varlist2"));
  
  if(scannerObj.scannerToken.tokenId == "SEMICOLON_tk")
  {
    getToken(scannerObj);
    return returnNode;
  }
  
  returnNode->child1 = varlist(scannerObj);

  return returnNode;
}


/* Function for the non-terminal stats in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <stats> -> <stat> <mStat>
 */
static std::unique_ptr<Node> stats(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("stats");
  std::unique_ptr<Node> returnNode(new Node("stats"));
  
  returnNode->child1 = stat(scannerObj);
  
  returnNode->child2 = mstat(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal mstat in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <mStat> -> empty | <stat> <mStat>
 */
static std::unique_ptr<Node> mstat(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("mstat");
  std::unique_ptr<Node> returnNode(new Node("mstat"));

  //set to make it easier to search if we have a statement keyword
  const std::unordered_set<std::string> STATS = {"read", "print", "start", "iff", "iterate", "set"};
  
  if(scannerObj.scannerToken.tokenId != "KEYWORD_tk") //Make sure its a keyword
    handleError("Statement Keyword", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);

  if(STATS.find(scannerObj.scannerToken.instance) != STATS.end())
  {
    returnNode->child1 = stat(scannerObj);
  
    returnNode->child2 = mstat(scannerObj);
    
    return returnNode;
  }
  
  
  //return returnNode; //Empty
  return nullptr;
}

/* Function for the non-terminal stat in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <stat> -> <read> | <print> | <block> | <cond> | <iter> | <assign>
 */
static std::unique_ptr<Node> stat(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("stat");
  std::unique_ptr<Node> returnNode(new Node("stat"));

  if(scannerObj.scannerToken.tokenId != "KEYWORD_tk")
    handleError("Statement Keyword", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);

  if(scannerObj.scannerToken.instance == "read") //Case 1
  {
    returnNode->child1 = read(scannerObj);
    return returnNode;
  } 
  else if(scannerObj.scannerToken.instance == "print") 
  {
    returnNode->child1 = print(scannerObj);
    return returnNode;
  } 
  else if(scannerObj.scannerToken.instance == "start") 
  {
    returnNode->child1 = block(scannerObj);
    return returnNode;
  } 
  else if(scannerObj.scannerToken.instance == "iff") 
  {
    returnNode->child1 = cond(scannerObj);
    return returnNode;
  } 
  else if(scannerObj.scannerToken.instance == "iterate") 
  {
    returnNode->child1 = iter(scannerObj);
    return returnNode;
  } 
  else if(scannerObj.scannerToken.instance == "set") 
  {
    returnNode->child1 = assign(scannerObj);
    return returnNode;
  }
     
  handleError("Statement Keyword", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  
  return returnNode;
}

/* Function for the non-terminal block in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <block> -> start <vars> <stats> stop
 */
static std::unique_ptr<Node> block(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("block");
  std::unique_ptr<Node> returnNode(new Node("block"));
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "start")
    handleError("start", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  returnNode->child1 = vars(scannerObj);
  
  returnNode->child2 = stats(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "stop")
    handleError("stop", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal read in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <read> -> read identifier ;
 */
static std::unique_ptr<Node> read(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("read");
  std::unique_ptr<Node> returnNode(new Node("read"));
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "read")
    handleError("read", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "ID_tk")
    handleError("Identifier", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  returnNode->tokens.push_back(scannerObj.scannerToken);
  
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "SEMICOLON_tk")
    handleError(";", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
    
    
  return returnNode;
}

/* Function for the non-terminal print in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <print> -> print <exp> ;
 */
static std::unique_ptr<Node> print(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("print");
  std::unique_ptr<Node> returnNode(new Node("print"));
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "print")
    handleError("print", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  returnNode->child1 = exp(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "SEMICOLON_tk")
    handleError(";", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal cond in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <cond> -> iff [ <exp> <relational> <exp> ] <stat>
 */
static std::unique_ptr<Node> cond(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("cond");
  std::unique_ptr<Node> returnNode(new Node("cond"));
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "iff")
    handleError("iff", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "LEFTBRACKET_tk")
    handleError("[", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj);
  
  returnNode->child1 = exp(scannerObj);
  
  returnNode->child2 = relational(scannerObj);
  
  returnNode->child3 = exp(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "RIGHTBRACKET_tk")
    handleError("]", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj); 
  
  returnNode->child4 = stat(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal iter in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <iter> -> iterate [ <exp> <relational> <exp> ] <stat>
 */
static std::unique_ptr<Node> iter(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("iter");
  std::unique_ptr<Node> returnNode(new Node("iter"));

  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "iterate")
    handleError("iterate", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "LEFTBRACKET_tk")
    handleError("[", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  returnNode->tokens.push_back(scannerObj.scannerToken);
  
  getToken(scannerObj);
  
  returnNode->child1 = exp(scannerObj);
  
  returnNode->child2 = relational(scannerObj);
  
  returnNode->child3 = exp(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "RIGHTBRACKET_tk")
    handleError("]", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj); 
  
  returnNode->child4 = stat(scannerObj);

  return returnNode;
}

/* Function for the non-terminal assign in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <assign> -> set identifier <exp> ;
 */ 
static std::unique_ptr<Node> assign(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("assign");
  std::unique_ptr<Node> returnNode(new Node("assign"));
  
  if(scannerObj.scannerToken.tokenId != "KEYWORDS_tk" && scannerObj.scannerToken.instance != "set")
    handleError("set", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "ID_tk")
    handleError("identifier", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  returnNode->tokens.push_back(scannerObj.scannerToken);
  
  getToken(scannerObj);
  
  returnNode->child1 = exp(scannerObj);
  
  if(scannerObj.scannerToken.tokenId != "SEMICOLON_tk")
    handleError(";", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  getToken(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal relational in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <relational> -> .le. | .ge. | .lt. | .gt. | ** | ~
 */
static std::unique_ptr<Node> relational(ScannerObj &scannerObj) 
{
  //std::unique_ptr<Node> returnNode = new Node("relational");
  std::unique_ptr<Node> returnNode(new Node("relational"));
  
  //Set to make it easier to search if the current token is a relational operator
  const std::unordered_set<std::string> RELATIONAL = 
    {"LESSEQUAL_tk", "LESSTHAN_tk", "GREATEREQUAL_tk", "GREATERTHAN_tk", "TILDE_tk", "ASTERISK_tk"};
  
  if(RELATIONAL.find(scannerObj.scannerToken.tokenId) == RELATIONAL.end())
    handleError("relational operator", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
    
  returnNode->tokens.push_back(scannerObj.scannerToken);
  getToken(scannerObj);

  return returnNode;
}


/* Function for the non-terminal exp in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <exp> -> <M> <exp2>
 */
static std::unique_ptr<Node> exp(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("exp");
  std::unique_ptr<Node> returnNode(new Node("exp"));
  
  returnNode->child1 = M(scannerObj);
  
  returnNode->child2 = exp2(scannerObj);

  return returnNode;
}

/* Function for the non-terminal exp2 in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <exp2> -> + <exp> | - <exp> | empty
 */
static std::unique_ptr<Node> exp2(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("exp2");
  std::unique_ptr<Node> returnNode(new Node("exp2"));
  
  if(scannerObj.scannerToken.tokenId == "PLUS_tk") //Case 1
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = exp(scannerObj);
    
    return returnNode;
  }
  else if(scannerObj.scannerToken.tokenId == "MINUS_tk") //Case 2
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = exp(scannerObj);
    
    return returnNode;
  }

  //return returnNode; //Empty
  return nullptr;
}

/* Function for the non-terminal M in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <M> -> <N> <M2>
 */
static std::unique_ptr<Node> M(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("M");
  std::unique_ptr<Node> returnNode(new Node("M"));
  
  returnNode->child1 = N(scannerObj);
  
  returnNode->child2 = M2(scannerObj);
  
  return returnNode;
}

/* Function for the non-terminal M2 in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <M2> -> % <M> | empty
 */
static std::unique_ptr<Node> M2(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("M2");
  std::unique_ptr<Node> returnNode(new Node("M2"));
  
  if(scannerObj.scannerToken.tokenId == "PERCENT_tk") //Case 1
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = M(scannerObj);
    
    return returnNode;
  }

  //return returnNode; //Empty
  return nullptr;
}

/* Function for the non-terminal N in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <N> -> <R> <N2> | - <N>
 */
static std::unique_ptr<Node> N(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("N");
  std::unique_ptr<Node> returnNode(new Node("N"));
  
  if(scannerObj.scannerToken.tokenId == "MINUS_tk") //Case 1
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = N(scannerObj);
    
    return returnNode;
  }
  //Case 2
  returnNode->child1 = R(scannerObj);
  
  returnNode->child2 = N2(scannerObj);

  return returnNode;
}

/* Function for the non-terminal N2 in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <N2> -> / <N> | empty
 */
static std::unique_ptr<Node> N2(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("N2");
  std::unique_ptr<Node> returnNode(new Node("N2"));
  
  if(scannerObj.scannerToken.tokenId == "FORWARDSLASH_tk") //Case 1
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = N(scannerObj);
    
    return returnNode;
  }

  //return returnNode; //Empty
  return nullptr;
}

/* Function for the non-terminal R in the BNF. Builds a node based on the structure of
 * the nonterminal. Returns the node made.
 * <R> -> ( <exp> ) | identifier | integer
 */
static std::unique_ptr<Node> R(ScannerObj &scannerObj)
{
  //std::unique_ptr<Node> returnNode = new Node("R");
  std::unique_ptr<Node> returnNode(new Node("R"));
  
  if(scannerObj.scannerToken.tokenId == "LEFTPAREN_tk") //Case 1
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    returnNode->child1 = exp(scannerObj);
    
    if(scannerObj.scannerToken.tokenId != "RIGHTPAREN_tk")
      handleError(")", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
      
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    return returnNode;
  }
  else if(scannerObj.scannerToken.tokenId == "ID_tk") //Case 2
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    return returnNode;
  }
  else if(scannerObj.scannerToken.tokenId == "INT_tk") //Case 3
  {
    returnNode->tokens.push_back(scannerObj.scannerToken);
    getToken(scannerObj);
    
    return returnNode;
  }
  else handleError("(, identifer, or integer", scannerObj.scannerToken.instance, scannerObj.scannerToken.line);
  
  return returnNode;
}
