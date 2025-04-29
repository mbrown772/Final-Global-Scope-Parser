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

#ifndef PARSER_H
#define PARSER_H

#include <memory>

#include "tree.h"


/*
 * Auxiliary function for the parser. Opens the file passed by FILENAME
 * and calls the first nonterminal in the BNF. Catches any invalid_argument
 * errors thrown by the program. Returns NULL if a error was found when parsing
 * or returns the root of the parse tree.
 */
std::unique_ptr<Node> parser(const std::string FILENAME);

#endif