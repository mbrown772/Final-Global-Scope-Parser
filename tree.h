#ifndef TREE_H
#define TREE_H

#include <vector>
#include <memory>

#include "language.h"

struct Node {
  std::string label; //What type of node this is
  std::vector<Token> tokens; //Holds all tokens of this non terminal
  std::unique_ptr<Node> child1 = nullptr; //Pointer for the first child node
  std::unique_ptr<Node> child2 = nullptr; //Pointer for the second child node
  std::unique_ptr<Node> child3 = nullptr; //Pointer for the third child node
  std::unique_ptr<Node> child4 = nullptr; //Pointer for the fourth child node
  Node(std::string s); //Constructor which is passed a string
};

/*
 * Prints the tree in pre order traversal
 * Is passed the root, initial level of 0.
 * Prints to cout.
 */
void printPreorder(const std::unique_ptr<Node>& NODE, int level);


#endif