//Target language information can be found here https://comp.umsl.edu/assembler/index. Programs can be run here

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "compiler.h"

static void exitError(const std::string S);
static void readInput(std::string tempFileName, std::string inputFileName);


int main(int argc, char *argv[]) 
{
  const std::string TEMPFILENAME = "temp.txt";
  
  //Program is only passed one argument
  if(argc > 2) exitError("Too many arguments");
  
  bool error;
  //Reading for stdin
  if(argc == 1) 
  {
    readInput(TEMPFILENAME, "");
    error = compile(TEMPFILENAME, "");
  } 
  //Reading from a file
  else
  {
    readInput(TEMPFILENAME, argv[1]);
    error = compile(TEMPFILENAME, argv[1]);
  }

  std::string printText = error ?  "Compilation Success" : "Compilation Failure";
  std::cout << printText << std::endl;
  
  remove(TEMPFILENAME.c_str());

  return 0;
}


/*
 *  Description: Helper function that exits the program on an error.
 *  Passed: Is passed a string to print.
 *  Return: Exits the program
 */
static void exitError(const std::string S) 
{
  std::cout << S << std::endl;
  exit(1);
}


/*
 *  Desription: Reads input from either stdin or a file saves read in input to temp.txt
 *  Passed: It is passed either a filename to print to and a file name to read from. "" for stdin.
 *  Return: Returns nothing
 */
static void readInput(std::string tempFileName, std::string inputFileName)
{
  std::ofstream fileOut; 
  fileOut.open(tempFileName.c_str());
  if (!fileOut.is_open()) exitError("Failed to open temp file!");
  
  std::string input;
  
  if(inputFileName.empty()) //Read from stdin 
  {
    while(std::getline(std::cin, input))
    {
      fileOut << input << std::endl;
    }
  } 
  else //Read from File
  {
    std::ifstream in;
    inputFileName += ".4280fs24";
    
    in.open(inputFileName.c_str());
    if(in.fail()) exitError("File does not exist! File must end with extension .4280fs24!");
    
    while(std::getline(in, input))
    {
      fileOut << input << std::endl;
    }
    in.close();
  }
  fileOut.close();
}
