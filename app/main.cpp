

#include "sauros/environment.hpp"
#include "sauros/front/parser.hpp"
#include "sauros/list.hpp"
#include "sauros/processor/processor.hpp"
#include "sauros/version.hpp"
#include <iostream>

namespace {
size_t line_no = 0;
std::shared_ptr<sauros::environment_c> env =
    std::make_shared<sauros::environment_c>();
sauros::processor_c proc;
} // namespace

// Add some environment variables to the system
//    - This serves as a good example of how to externally extend the language
//
void setup_env() {
   env.get()->set("__version",
                  sauros::cell_c([=](std::vector<sauros::cell_c> &cells,
                                     std::shared_ptr<sauros::environment_c> env)
                                     -> std::optional<sauros::cell_c> {
                     if (cells.size() != 1) {
                        throw sauros::processor_c::runtime_exception_c(
                            "`__version` expects no arguments, but " +
                                std::to_string(cells.size() - 1) +
                                " were given",
                            cells[0].location);
                     }

                     return {sauros::cell_c(sauros::cell_type_e::STRING,
                                            std::string(LIBSAUROS_VERSION),
                                            cells[0].location)};
                  }));
}

void run(std::string line) {
   auto result = sauros::parser::parse_line("test", line_no++, line);

   if (result.error_info) {
      std::cout << result.error_info.get()->message << std::endl;
   }

   auto cell = proc.process(result.cell, env);

   if (cell.has_value()) {
      std::string stringed_cell;
      proc.cell_to_string(stringed_cell, (*cell), env);
      std::cout << stringed_cell << std::endl;
   }
}

int main(int argc, char **argv) {

   setup_env();

   // run("[var y [lambda x [put x]]]");
   // run("[var x 3]");
   // run("[put \"x =\" x");
   // run("[var f [lambda [x y z] [put \"You called it:\" x y z]]]");
   // run("[f 1 2 3]");
   // run("[f x 9 10]");

   run("[__version]");

   run("[var x [list 3 [list 5 6 4] 5]]");
   run("[ x ]");
   run("[put \"x = \" x]");
   run("[var a::x_size [len x]]");
   run("[a::x_size]");

   return 0;
}