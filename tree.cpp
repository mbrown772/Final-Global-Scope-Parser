#include <iostream>

#include "tree.h"
#include "language.h"

//Constructor for a Node when passed a string
Node::Node(std::string s) 
{
  this->label = s; 
  this->tokens.clear(); 
  this->child1 = nullptr; 
  this->child2 = nullptr; 
  this->child3 = nullptr; 
  this->child4 = nullptr; 
}

/*
 * Prints the tree in pre order traversal
 * Is passed the root, initial level of 0.
 * Prints to cout.
 */
void printPreorder(const std::unique_ptr<Node>& NODE, int level)
{
  if(NODE != nullptr) 
  {
    //Add 2 blank spaces per level
    for(int i = 0; i < level; i++) 
      std::cout << "  ";
    
    std::cout << NODE->label << ": ";
    
    for(size_t i = 0; i < NODE->tokens.size(); i++) //Print all tokens
    {
      std::cout << NODE->tokens[i].tokenId << " = " << NODE->tokens[i].instance << "|";
    
    }
    std::cout << std::endl;
    
    printPreorder(NODE->child1, level+1);
    printPreorder(NODE->child2, level+1);
    printPreorder(NODE->child3, level+1);
    printPreorder(NODE->child4, level+1);
  }
}
