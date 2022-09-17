#include "load.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace app {

int load_c::run(const std::string &file,
                std::shared_ptr<sauros::environment_c> &env) {

   std::fstream fs;
   fs.open(file, std::fstream::in);
   if (!fs.is_open()) {
      std::cerr << "Unable to open file : " << file << std::endl;
      return 1;
   }

   std::string line;
   uint64_t line_number{0};
   while (std::getline(fs, line)) {
      line_number++;

      auto buffer = _buffer.submit(line);
      if (!buffer.has_value()) {
         continue;
      }

      auto parser_result =
          sauros::parser::parse_line(file.c_str(), line_number, (*buffer));

      if (parser_result.result == sauros::parser::result_e::ERROR) {
         std::cerr << "Error parsing line";
         if (parser_result.error_info) {
            std::cout << ": " << parser_result.error_info->message;
         }
         std::cout << std::endl;
         return 1;
      }

      try {
         _list_processor.process(parser_result.cell, env);

      } catch (sauros::processor_c::runtime_exception_c &e) {
         std::cout << e.what() << std::endl;
         std::exit(1);
      } catch (sauros::processor_c::assertion_exception_c &e) {
         std::cout << e.what() << std::endl;
         std::exit(1);
      } catch (sauros::environment_c::unknown_identifier_c &e) {
         std::cout << e.what() << " : " << e.get_id() << std::endl;
         std::exit(1);
      }
   }

   return 0;
}

} // namespace app