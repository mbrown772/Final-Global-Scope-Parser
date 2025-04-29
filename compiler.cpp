#include <iostream>
#include <fstream>

#include "compiler.h"
#include "tree.h"
#include "parser.h"
#include "statSem.h"

static void genTarget(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);

//Conditional and iteration
static void cond(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);
static void iter(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);
static std::string getRelationString(const std::string relatOp, const std::string label);

//Expression nodes
static void handleExp(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);
static void M(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);
static void N(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);
static void R(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut);

static std::string genTempVar(std::unique_ptr<SemanticTable>& table);
static std::string genBranchLabel();


/*
 *  Description: This function parses a given FILENAME and saves it as BUILDNAME.asm if given a string other than "" otherwise a.asm.
 *               FILENAME needs to have /n at the end of each line to count lines. FILENAME is then run through parser for a parse tree.
 *               If there is any chracters not in the grammer or bad code structure a error is printed to the screen and returns false.
 *               Static semantics are then run on the parse tree. If static semantics fail a error is printed to the screen return false.
 *               A target is then generated from the confirmed good file. The target is generated in UMSL's ASM interpreter langauge.
 *  Passed:      A string FILENAME to read from. A string BUILDNAME to save as. If BUILDNAME is empty default is a.asm.
 *  Returns:     The status of the parse.
 */
bool compile(const std::string FILENAME, const std::string BUILDNAME)
{
  //Check input program and build parse tree (parser.h)
  std::unique_ptr<Node> parseRoot = parser(FILENAME); 
  if(parseRoot == nullptr)
  {
    std::cout << "ERROR Parse Failure" << std::endl;
    return false;
  }
  
  //Check Semantics and build semantic table
  std::unique_ptr<SemanticTable> semTable = buildTable(parseRoot);
  if(semTable == nullptr)
  {
    std::cout << "ERROR Static Semantics Failure" << std::endl;
    return false;
  }
  
  //Create and open the target file
  std::string buildName = "a.asm";
  if(!BUILDNAME.empty()) buildName = BUILDNAME + ".asm";
  std::ofstream fileOut(buildName.c_str());
  if (!fileOut.is_open()) 
  {
    std::cout << "Failed to open build file!" << std::endl;
    return false;
  }
  
  //Generate the targets code
  genTarget(parseRoot, semTable, fileOut);
  fileOut << "STOP" << std::endl;
  semTable->tableOut(fileOut);
  
  fileOut.close();
  
  return true;
}

/*
 * Description: Prints to the target file the conversion of the input language recursively. Generates in UMSL ASM interperter language.
 *              Nodes are expected to have the child(1|2|3|4) be in order of appearnce for that specific node based on the BNF.
 * Passed:      NODE -> root of the parse tree | table -> the semantic table | fileOut -> output filestream already opened
 */
static void genTarget(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut) 
{
  if(NODE == nullptr) return;
  
  //<program> -> program <vars> <block>
  if(NODE->label == "program") //NO CODE GEN
  {
    //We don't care about vars here that was handled when making the semantic table
    genTarget(NODE->child2, table, fileOut);
    return;
  }
  //<stats> -> <stat> <mStat>
  else if(NODE->label == "stats") //NO CODE GEN
  {
    genTarget(NODE->child1, table, fileOut); //<stat>
    genTarget(NODE->child2, table, fileOut); //<mstat>
    return;
  }
  //<mStat> -> empty | <stat> <mStat>
  else if(NODE->label == "mstat") //No CODE GEN
  {
    genTarget(NODE->child1, table, fileOut); //<stat>
    genTarget(NODE->child2, table, fileOut); //<mstat>
    return;
  }
  //<stat> -> <read> | <print> | <block> | <cond> | <iter> | <assign>
  else if(NODE->label == "stat") //NO CODE GEN
  {
    genTarget(NODE->child1, table, fileOut); //<read> | <print> | <block> | <cond> | <iter> | <assign>
    return;
  }
  //<block> -> start <vars> <stats> stop
  else if(NODE->label == "block") //NO CODE GEN
  {
    //We don't care about vars here that was handled when making the semantic table
    genTarget(NODE->child2, table, fileOut); //<stats>
    return;
  }
  //<read> -> read identifier ;
  else if(NODE->label == "read")
  {
    fileOut << "READ " << NODE->tokens[0].instance << std::endl;
    return;
  }
  //<print> -> print <exp> ;
  else if(NODE->label == "print")
  {
    genTarget(NODE->child1, table, fileOut); //<exp>
    std::string tempVar = genTempVar(table);
    fileOut << "STORE " << tempVar << std::endl;
    fileOut << "WRITE " << tempVar << std::endl;
    return;
  }
  else if(NODE->label == "cond")
  {
    cond(NODE, table, fileOut);
    return;
  }
  else if(NODE->label == "iter")
  {
    iter(NODE, table, fileOut);
    return;
  }
  //<assign> -> set identifier <exp> ;
  else if(NODE->label == "assign")
  {
    genTarget(NODE->child1, table, fileOut); //<exp>
    fileOut << "STORE " << NODE->tokens[0].instance << std::endl;
    return;
  }
  else if(NODE->label == "exp")
  {
    handleExp(NODE, table, fileOut);
    return;
  }
  
  //If we get here someone broke the parser
  std::cout << "SOMEONE BROKE THE PARSER NODE LABEL IS: " << NODE->label << std::endl;
}



/////////////////////////conditional + iteration/////////////////////////////////////////////
/* Description: Handles <cond> and creates a c style if wihout else. If the condition in the loop is correct we don't skip.           
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 * <cond> -> iff [ <exp> <relational> <exp> ] <stat>
 */
static void cond(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
  genTarget(NODE->child3, table, fileOut); //right <exp>
  std::string tempVarRight = genTempVar(table);
  fileOut << "STORE " << tempVarRight << std::endl;
  
  genTarget(NODE->child1, table, fileOut); //left <exp> saved in acc
  
  std::string branchLabel = genBranchLabel();
  std::string branchCode = getRelationString(NODE->child2->tokens[0].tokenId, branchLabel); //Get the realtional token name
  
  fileOut << "SUB " << tempVarRight << std::endl;
  fileOut << branchCode << std::endl;
  
  genTarget(NODE->child4, table, fileOut); //<stat>
  
  fileOut << branchLabel << ": NOOP" << std::endl;

}

/* Description: Handles <iter> and creates a c style while loop. Creates a branch to return at the top. If the condition 
 *              in the loop is correct we don't skip.               
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 * <iter> -> iterate [ <exp> <relational> <exp> ] <stat>
 */
static void iter(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
  std::string topBranchLabel = genBranchLabel();
  fileOut << topBranchLabel << ": NOOP" << std::endl;
  
  genTarget(NODE->child3, table, fileOut); //right <exp>
  std::string tempVarRight = genTempVar(table);
  fileOut << "STORE " << tempVarRight << std::endl;
  
  genTarget(NODE->child1, table, fileOut); //left <exp> saved in acc
  
  std::string condBranchLabel = genBranchLabel();
  std::string branchCode = getRelationString(NODE->child2->tokens[0].tokenId, condBranchLabel); //Get the relational token name
  
  fileOut << "SUB " << tempVarRight << std::endl;
  fileOut << branchCode << std::endl;
  
  genTarget(NODE->child4, table, fileOut); //<stat>
  
  fileOut << "BR " << topBranchLabel << std::endl;
  fileOut << condBranchLabel << ": NOOP" << std::endl;
}

/*
 * Description: The string return are the oppisite of what is given. This is because we are skipping the code below on a bade relation.
 * Passed:      relatOP is the token label relating to that relation operator. label is the label for branching.
 * Returns:     A string with the branches matching the given relation operator.
 */
static std::string getRelationString(const std::string relatOp, const std::string label)
{
  std::string returnString = "";
  if(relatOp == "LESSEQUAL_tk") // <=
  {
     returnString += "BRPOS " + label;
  }
  else if(relatOp == "LESSTHAN_tk") // <
  {
    returnString += "BRZPOS " + label;
  }
  else if(relatOp == "GREATEREQUAL_tk") //>=
  {
    returnString += "BRNEG " + label;
  }
  else if(relatOp == "GREATERTHAN_tk") //>
  {
    returnString += "BRZNEG " + label;
  }
  else if(relatOp == "TILDE_tk") // !=
  {
    returnString += "BRZERO " + label;
  }
  else // ==
  {
    returnString += "BRPOS " + label;
    returnString += "\nBRNEG " + label;
  }
  
  return returnString;
}
//////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////Expression//////////////////////////////////////////////////////////
/*
 * Description: Handles a full expression of any length. This is a combination of exp and exp2. The result of this expression is left 
 *              in the accumulator(acc) and is expected to be handled by whatever node needs the value.
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 * <exp>  -> <M> <exp2>
 * <exp2> -> + <exp> | - <exp> | empty
 */
static void handleExp(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
  if(NODE->child2 != nullptr) //<M> (+ <exp> | - <exp>) if <exp2> exists must be one of these two
  {
    handleExp(NODE->child2->child1, table, fileOut); //the <exp> in <exp2>
    
    std::string tempVarEXP = genTempVar(table);
    fileOut << "STORE " << tempVarEXP << std::endl;
    
    M(NODE->child1, table, fileOut); // <M>
    
    if(NODE->child2->tokens[0].tokenId == "PLUS_tk")
    {
      fileOut << "ADD " << tempVarEXP << std::endl;
    }
    else //MINUS_tk
    {
      fileOut << "SUB " << tempVarEXP << std::endl;
    }
    return;
  }
  else //<M>
  {
    M(NODE->child1, table, fileOut);
    return;
  }
}

/*
 * Description: Handles the M and M2 nodes in the tree. This only appears in expression and is expected to be called in handleExp.
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 * <M>  -> <N> <M2>
 * <M2> -> % <M> | empty
 */
static void M(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
  if(NODE->child2 != nullptr) //<N> % <M> (if there is a child it always goes to % <M> 
  {
    M(NODE->child2->child1, table, fileOut); // <M>
    std::string tempVarM = genTempVar(table);
    fileOut << "STORE " << tempVarM << std::endl;
    
    N(NODE->child1, table, fileOut); // <N>
    
    fileOut << "MULT " << tempVarM << std::endl; 
    
    return;
  }
  else //<N>
  {
    N(NODE->child1, table, fileOut); //<N>
    
    return;
  }
}

/*
 * Description: Handles the N and N2 nodes in the tree. This only appears in expression and is expected to be called in handleExp.
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 * <N>  -> <R> <N2> | - <N>
 * <N2> -> / <N> | empty
 */
static void N(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
  if(!NODE->tokens.empty() ) // - <N>
  {
    N(NODE->child1, table, fileOut);
    fileOut << "MULT -1" << std:: endl;
    return;
  }
  else if(NODE->child2 != nullptr) //<R> / <N> (if this child <N2> exists it always goes to n)
  {
    N(NODE->child2->child1, table, fileOut); //<N>
    
    std::string tempVarN = genTempVar(table);
    fileOut << "STORE " << tempVarN << std::endl;
    
    R(NODE->child1, table, fileOut); //<R>
    
    fileOut << "DIV " << tempVarN << std::endl;
    
    return;
  }
  else //MUST GO TO just <R>
  {
    R(NODE->child1, table, fileOut);
    
    return;
  }
  
}

/*
 *  Description: Handles a R node in the tree. This only appears in a expression and is expected to be called in handleExp
 * Passed: NODE -> the <expr> node in the tree | table -> the semantic table to add temps to |
 *                 fileOut -> output filestream already opened
 *  <R> -> ( <exp> ) | identifier | integer
 */
static void R(const std::unique_ptr<Node>& NODE, std::unique_ptr<SemanticTable>& table, std::ofstream& fileOut)
{
    if(NODE->child1 != nullptr)
    {
      handleExp(NODE->child1, table, fileOut);
      return;
    }
    fileOut << "LOAD " << NODE->tokens[0].instance << std::endl;
    
    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////

/* Description: Creates a new temp variable name in the form of _(num) in incremental order. The number is then 
 *              added to the semantic table to be printed
 *              at the end of the file.
 * Passed: The semantic table so we can add the new variable to it.
 */
static std::string genTempVar(std::unique_ptr<SemanticTable>& table)
{
  static int tempVarNum = 0;
  std::string returner = "_" + std::to_string(tempVarNum);
  table->insert(returner, -1);
  tempVarNum++;
  return returner;
}

/*
 * Description: Creates a new branch lable int he form of B(num) in incremental order.
 */
static std::string genBranchLabel()
{
  static int tempLabelNum = 0;
  std::string returner = "B" + std::to_string(tempLabelNum);
  tempLabelNum++;
  return returner;
}






















