# Creating Predictive Parser using c++
* Clone the repository and excute the following commands.
* g++ parser.cpp
* cat input.txt | ./myScan | ./a.out
* In parser.cpp, for any user provided grammar, left recursion (if present) is removed by "eleminate_left_recursion" function .
* Further get_first_nonterminal calculates the FIRST set for all the non-terminals.
* get_first_terminals calculates the FIRST set for all the terminals.
* Follow calculates FOLLOW set for each terminal and non terminal.
* parsing table calculates the parsing table for the given grammar.
* create_predictive_parser creates the general predictive parser for grammar.