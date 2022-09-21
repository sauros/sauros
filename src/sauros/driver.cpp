#include "driver.hpp"
#include "front/parser.hpp"
#include <filesystem>
#include <iostream>
#include "rang.hpp"

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
      parser_error(parser_result.error_info->message, parser_result.error_info->location);
      return;
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

int file_executor_c::run(const std::string &file) {\

   _file = file;
   _fs.open(_file, std::fstream::in);
   if (!_fs.is_open()) {
      std::cerr << "Unable to open file : " << _file << std::endl;
      return 1;
   }

   std::string line;
   uint64_t line_number{0};
   while (std::getline(_fs, line)) {
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
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_location());
   std::exit(1);
}

void file_executor_c::except(sauros::processor_c::assertion_exception_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_location());
   std::exit(1);
}

void file_executor_c::except(sauros::environment_c::unknown_identifier_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_location());
   std::exit(1);
}

void file_executor_c::parser_error(std::string& e, location_s location) {
   std::cout << rang::fg::red << e << rang::fg::reset << std::endl;
   display_error_from_file(location);
   std::exit(1);
}

void file_executor_c::display_error_from_file(location_s location) {

   std::cout << rang::fg::magenta << _file << rang::fg::reset << " : (" << rang::fg::blue << location.line << rang::fg::reset << "," << rang::fg::blue << location.col << rang::fg::reset << ")\n";

   struct line_data_pair_s {
      uint64_t number;
      std::string data;
   };

   // Bring the file back to the beginning
   _fs.seekg(0, std::ios::beg);

   // A window of source
   std::vector<line_data_pair_s> window;

   // Get to the line
   std::string line_data;
   uint64_t line_number{0};

   // Determine the upper and lower bound for a source code window
   int64_t upper_bound = location.line + 4;
   int64_t lower_bound = (int64_t)location.line - 5; 
   if (lower_bound < 0) {
      lower_bound = 0;
   }

   // Build a window of source code to display
   while (std::getline(_fs, line_data)) {

      line_number++;
      if ((line_number >= lower_bound && lower_bound < location.line) || 
            location.line == line_number ||
            line_number > location.line && line_number < upper_bound) {
         window.push_back({
            .number = line_number,
            .data = line_data
         });
      }
      
      if (line_number >= upper_bound) {
         break;
      }
   }

   // Determine the alignment
   size_t width = 2;
   {
      auto s = std::to_string(upper_bound);
      if (s.length()+1 > width) {
         width = s.length() + 1;
      } 
   }

   // Make an arrow to show where the error is
   std::string pointer;
   for (size_t i = 0; i < location.col; i++) {
      pointer += "~";
   }
   pointer += "^";

   // Draw the window
   for(auto line_data : window) {
      if (line_data.number == location.line) {
         std::cout << rang::fg::yellow << std::right << std::setw(width) << line_data.number << rang::fg::reset << " | " << line_data.data << std::endl;
         std::cout << rang::fg::cyan << std::right << std::setw(width) << ">>" << rang::fg::reset << " | " << rang::fg::red << pointer << rang::fg::reset << std::endl;
      } else {
         std::cout << rang::fg::green << std::right << std::setw(width) << line_data.number << rang::fg::reset << " | " << line_data.data << std::endl;
      }
   }

}

void repl_c::start() {

   uint64_t line_number{0};
   bool show_prompt{true};
   std::string buffer;

   uint64_t tabs = 0;
   while (_do) {

      line_number++;

      if (show_prompt) {
         std::cout << rang::fg::cyan << "> " << rang::fg::reset;
      } else {
         for (auto i = 0; i < tabs; i++) {
            std::cout << "  ";
         }
      }

      std::string line;
      std::getline(std::cin, line);

      if (line.empty()) {
         continue;
      }

      auto buffer = _buffer->submit(line);
      if (buffer.has_value()) {

         execute("repl", line_number, (*buffer));
         show_prompt = true;
         tabs = 0;

      } else {
         show_prompt = false;
         tabs++;
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
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::except(sauros::processor_c::assertion_exception_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::except(sauros::environment_c::unknown_identifier_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::parser_error(std::string& e, location_s location) {
   std::cout << rang::fg::red << e << rang::fg::reset << std::endl;
}

} // namespace sauros