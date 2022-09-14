

#include <iostream>
#include "sauros/list.hpp"
#include "sauros/front/parser.hpp"
#include "sauros/processor/processor.hpp"
#include "sauros/environment.hpp"

namespace {
   size_t line_no = 0;
   std::shared_ptr<sauros::environment_c> env = std::make_shared<sauros::environment_c>();
   sauros::processor_c proc(env);
}

void run(std::string line) {
   auto result = sauros::parser::parse_line(env, "test", line_no++, line);

   if (result.error_info) {
      std::cout << result.error_info.get()->message << std::endl;
   }

   auto proc_result = proc.process(result.cell);
}

int main(int argc, char** argv) {

   //run("[var y [lambda x [put x]]]");
   run("[var x 3]");
   run("[put \"x =\" x");
   run("[var f [lambda [t] [put \"You called it:\" t]]]");

   run("[f 4]");

   return 0;
}