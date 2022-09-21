#include "driver.hpp"
#include "front/parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace sauros {

class input_buffer_c {
 public:
   std::optional<std::string> submit(std::string &data);

 private:
   std::string _buffer;
   uint64_t _tracker{0};
};

std::optional<std::string> input_buffer_c::submit(std::string &line) {

   // Remove comments
   std::size_t comment_loc = line.find_first_of(";");
   if (comment_loc != std::string::npos) {
      line = line.substr(0, comment_loc);
   }

   if (line.empty()) {
      return {};
   }

   // Walk the line and see if the given brackets
   // indicate that we have a full
   for (auto &c : line) {
      if (c == '[') {
         _tracker++;
      } else if (c == ']') {
         _tracker--;
      }
      _buffer += c;
   }

   // If we have a statement and all brackets are closed then
   // we can submit the statement
   if (_tracker == 0 && !_buffer.empty()) {
      std::string ret_buffer = _buffer;
      _buffer.clear();
      return {ret_buffer};
   } else {
      _buffer += ' ';
   }

   return {};
}

driver_if::driver_if(std::shared_ptr<sauros::environment_c> &env) : _env(env) {
   _buffer = new input_buffer_c();
}

driver_if::~driver_if() { delete _buffer; }

void driver_if::execute(const char *source, uint64_t line_number,
                        std::string &line) {

   auto parser_result = sauros::parser::parse_line(source, line_number, line);

   if (parser_result.result == sauros::parser::result_e::ERROR) {
      std::cerr << "Error parsing line";
      if (parser_result.error_info) {
         std::cout << ": " << parser_result.error_info->message;
      }
      std::cout << std::endl;
      std::exit(1);
   }

   try {
      auto result = _list_processor.process(parser_result.cell, _env);
      if (result.has_value()) {
         cell_returned((*result));
      }
   } catch (sauros::processor_c::runtime_exception_c &e) {
      except(e);
   } catch (sauros::processor_c::assertion_exception_c &e) {
      except(e);
   } catch (sauros::environment_c::unknown_identifier_c &e) {
      except(e);
   }
}

int file_executor_c::run(const std::string &file) {
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

      auto buffer = _buffer->submit(line);
      if (!buffer.has_value()) {
         continue;
      }

      execute(file.c_str(), line_number, (*buffer));
   }
   return 0;
}

void file_executor_c::cell_returned(cell_c &cell) { /* Not needed */
}

void file_executor_c::except(sauros::processor_c::runtime_exception_c &e) {
   std::cout << e.what() << std::endl;
   std::exit(1);
}

void file_executor_c::except(sauros::processor_c::assertion_exception_c &e) {
   std::cout << e.what() << std::endl;
   std::exit(1);
}

void file_executor_c::except(sauros::environment_c::unknown_identifier_c &e) {
   std::cout << e.what() << " : " << e.get_id() << std::endl;
   std::exit(1);
}

void repl_c::start() {

   uint64_t line_number{0};
   bool show_prompt{true};
   std::string buffer;
   while (_do) {

      line_number++;

      if (show_prompt) {
         std::cout << "> ";
      }

      std::string line;
      std::getline(std::cin, line);

      auto buffer = _buffer->submit(line);
      if (buffer.has_value()) {

         execute("repl", line_number, (*buffer));
      }
   }
}

void repl_c::stop() { _do = false; }

void repl_c::cell_returned(cell_c &cell) {
   std::string s_cell;
   _list_processor.cell_to_string(s_cell, cell, _env, true);
   std::cout << s_cell << std::endl;
}

void repl_c::except(sauros::processor_c::runtime_exception_c &e) {
   std::cout << e.what() << std::endl;
}

void repl_c::except(sauros::processor_c::assertion_exception_c &e) {
   std::cout << e.what() << std::endl;
}

void repl_c::except(sauros::environment_c::unknown_identifier_c &e) {
   std::cout << e.what() << " : " << e.get_id() << std::endl;
}
} // namespace sauros