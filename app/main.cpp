

#include <iostream>
#include "sauros/list.hpp"
#include "sauros/parser.hpp"

namespace{
class visitor : public sauros::cell_visitor_c {
public:
   virtual void accept(sauros::symbol_c &cell) override final {
      std::cout << cell.data << " ";
   }

   virtual void accept(sauros::list_c &cell) override final {
      std::cout << "[ ";
      for (auto& c : cell.cells ) {
         c.get()->visit(*this);
      }
      std::cout << "] ";
   }

   virtual void accept(sauros::string_c &cell) override final {
      std::cout << cell.data << " ";
   }

   virtual void accept(sauros::integer_c &cell) override final {
      std::cout << cell.data << " ";
   }

   virtual void accept(sauros::double_c &cell) override final {
      std::cout << cell.data << " ";
   }

};
}

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

   visitor cell_visitor;

   //auto result = sauros::parser::parse_line("test", 8, "[define x [+ [- 10 2] 10]]");
   auto result = sauros::parser::parse_line("test", 8, "[block [c] [[def] d [+ 3 1] ] ]");

   if (result.error_info) {
      std::cout << result.error_info.get()->message << std::endl;
   }

   std::cout << "[ ";
   if (result.list ) {
      for (auto& c : result.list.get()->cells){
         c->visit(cell_visitor);
      }
   }
   std::cout << "]" << std::endl;

   return 0;
}