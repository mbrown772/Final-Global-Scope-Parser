//Target language information can be found here https://comp.umsl.edu/assembler/index. Programs can be run here

#ifndef COMPILER_H
#define COMPILER_H

#include <string>

/*
 *  Description: This function parses a given FILENAME and saves it as BUILDNAME.asm if given a string other than "" otherwise a.asm.
 *               FILENAME needs to have /n at the end of each line to count lines. FILENAME is then run through parser for a parse tree.
 *               If there is any chracters not in the grammer or bad code structure a error is printed to the screen and returns false.
 *               Static semantics are then run on the parse tree. If static semantics fail a error is printed to the screen return false.
 *               A target is then generated from the confirmed good file. The target is generated in UMSL's ASM interpreter langauge.
 *  Passed:      A string FILENAME to read from. A string BUILDNAME to save as. If BUILDNAME is empty default is a.asm.
 *  Returns:     The status of the parse.
 */
bool compile(const std::string FILENAME, const std::string BUILDNAME);

#endif