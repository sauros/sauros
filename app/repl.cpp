#include "repl.hpp"

#include <iostream>

namespace app {

void repl_c::start() {

   uint64_t line_number{0};
   bool show_prompt{true};
   std::string buffer;
   while (_do) {

      line_number++;

      if (show_prompt){
         std::cout << "> ";
      }

      std::string line;
      std::getline(std::cin, line);

      auto buffer = _buffer.submit(line);
      if (buffer.has_value()) {
         
         run(line_number, (*buffer));
      }
   }
}

void repl_c::stop() {
   _do = false;
}

void repl_c::run(const uint64_t line_number, std::string& line) {
   
   auto parser_result = sauros::parser::parse_line("repl", line_number, line);

   if (parser_result.result == sauros::parser::result_e::ERROR) {
      std::cerr << "Error parsing line";
      if (parser_result.error_info) {
         std::cout << ": " << parser_result.error_info->message;
      }
      std::cout << std::endl;
      return;
   }

   try {
      auto cell = proc.process(parser_result.cell, _env);
      if (cell.has_value()) {
         std::string s_cell;
         proc.cell_to_string(s_cell, (*cell), _env, true);
         std::cout << s_cell << std::endl;
         return;
      }
      std::cout << std::endl;

   } catch (sauros::processor_c::runtime_exception_c& e) {
      std::cout << e.what() << std::endl;
   } catch (sauros::environment_c::unknown_identifier_c& e) {
      std::cout << e.what() << " : " << e.get_id() << std::endl;
   }
}

} // namespace app