

#include <iostream>
#include "sauros/list.hpp"
#include "sauros/front/parser.hpp"
#include "sauros/processor/processor.hpp"
#include "sauros/environment.hpp"

namespace {
   size_t line_no = 0;
   std::shared_ptr<sauros::environment_c> env = std::make_shared<sauros::environment_c>();
   sauros::processor_c proc;
}

void run(std::string line) {
   auto result = sauros::parser::parse_line(env, "test", line_no++, line);

   if (result.error_info) {
      std::cout << result.error_info.get()->message << std::endl;
   }

   auto proc_result = proc.process(result.cell, env);

   if (proc_result.returned_value.has_value()) {
      std::string stringed_cell;
      proc.cell_to_string(stringed_cell, (*proc_result.returned_value), env);
      std::cout << stringed_cell << std::endl;
   }
}

int main(int argc, char** argv) {

   //run("[var y [lambda x [put x]]]");
  //run("[var x 3]");
  //run("[put \"x =\" x");
  //run("[var f [lambda [x y z] [put \"You called it:\" x y z]]]");
  //run("[f 1 2 3]");
  //run("[f x 9 10]");

   run("[var x [list 3 [list 5 6 4] 5]]");
   run("[ x ]");
   run("[put \"x = \" x]");
   run("[var x_size [len x]]");
   run("[x_size]");

   return 0;
}