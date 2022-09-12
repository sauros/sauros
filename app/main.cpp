

#include <iostream>
#include "sauros/parser.hpp"

int main(int argc, char** argv) {

/*
   { auto result = sauros::parser::parse_line("test", 0, "(define x (+ 1 (- 10 7)))", true); }
   { auto result = sauros::parser::parse_line("test", 1, "(define x 430)", true); }
   { auto result = sauros::parser::parse_line("test", 2, R"((define x "Some string"))", true); }
   { auto result = sauros::parser::parse_line("test", 3, R"((define x "Some \"other\" string"))", true); }
   { auto result = sauros::parser::parse_line("test", 4, R"((define x "Some\" string"))", true); }
   { auto result = sauros::parser::parse_line("test", 6, "(+ 3 4)", true); }
   { auto result = sauros::parser::parse_line("test", 7, "(define x (- 10 3))", true); }
*/



   { auto result = sauros::parser::parse_line("test", 8, "(block (define 1) (define y 2) (define z 5) )", true); }
   return 0;
}