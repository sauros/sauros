#include "driver.hpp"
#include "profiler.hpp"
#include "rang.hpp"
#include <csignal>
#include <filesystem>
#include <iostream>
#include <sauros/linenoise/linenoise.hpp>
#include <sauros/system/system.hpp>
#include <unordered_map>

namespace sauros {

void display_error_from_file(std::shared_ptr<std::string> file,
                             const location_s *location) {

   std::string file_actual = (file) ? (*file) : "unknown";

   if (!location) {
      std::cout << rang::fg::red
                << "No location object given - perhaps an unhandled issue in "
                   "an external library caused an issue\n\n"
                << rang::fg::reset << std::endl;
      std::cout << rang::fg::cyan
                << "Please report this to the github.com/bosley/sauros issues "
                   "page with a copy of the source that caused the problem so "
                   "the method utilizing locations from external cells can be "
                   "updated to catch the issue(s)\n\n"
                << rang::fg::reset << std::endl;
      return;
   }

   std::cout << rang::fg::magenta << file_actual << rang::fg::reset << " : ("
             << rang::fg::blue << location->line << rang::fg::reset << ","
             << rang::fg::blue << location->col << rang::fg::reset << ")\n";

   struct line_data_pair_s {
      uint64_t number;
      std::string data;
   };

   std::ifstream fs;
   fs.open(file_actual);

   if (!fs.is_open()) {
      return;
   }

   // A window of source
   std::vector<line_data_pair_s> window;

   // Get to the line
   std::string line_data;
   uint64_t line_number{0};

   // Determine the upper and lower bound for a source code window
   int64_t upper_bound = location->line + 4;
   int64_t lower_bound = (int64_t)location->line - 5;
   if (lower_bound < 0) {
      lower_bound = 0;
   }

   // Build a window of source code to display
   while (std::getline(fs, line_data)) {
      line_number++;
      if ((line_number >= lower_bound && lower_bound < location->line) ||
          location->line == line_number ||
          line_number > location->line && line_number < upper_bound) {
         window.push_back({.number = line_number, .data = line_data});
      }

      if (line_number >= upper_bound) {
         break;
      }
   }

   // Determine the alignment
   size_t width = 2;
   {
      auto s = std::to_string(upper_bound);
      if (s.length() + 1 > width) {
         width = s.length() + 1;
      }
   }

   // Make an arrow to show where the error is
   std::string pointer;
   for (size_t i = 0; i < location->col; i++) {
      pointer += "~";
   }
   pointer += "^";

   // Draw the window
   for (auto line_data : window) {
      if (line_data.number == location->line) {
         std::cout << rang::fg::yellow << std::right << std::setw(width)
                   << line_data.number << rang::fg::reset << " | "
                   << line_data.data << std::endl;
         std::cout << rang::fg::cyan << std::right << std::setw(width) << ">>"
                   << rang::fg::reset << " | " << rang::fg::red << pointer
                   << rang::fg::reset << std::endl;
      } else {
         std::cout << rang::fg::green << std::right << std::setw(width)
                   << line_data.number << rang::fg::reset << " | "
                   << line_data.data << std::endl;
      }
   }
   fs.close();
}

class input_buffer_c {
 public:
   std::optional<std::string> submit(std::string &data);

 private:
   std::string _buffer;
   uint64_t _tracker{0};
};

std::optional<std::string> input_buffer_c::submit(std::string &line) {

   // Remove comments
   parser::remove_comments(line);

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
   }

   return {};
}

driver_if::driver_if(std::shared_ptr<sauros::environment_c> env) : _env(env) {
   _buffer = new input_buffer_c();
}

driver_if::~driver_if() { delete _buffer; }

void driver_if::indicate_complete() {
   try {
      _segment_parser.indicate_complete();
   } catch (sauros::parser::parser_exception_c &e) {
      except(e);
   }
}

void file_executor_c::finish() {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("file_executor_c::finish");
   profiler_c::get_profiler()->dump();
#endif
}

void driver_if::execute(parser::segment_parser_c::segment_s segment) {
   try {
      auto parser_result = _segment_parser.submit(segment);
      if (!parser_result.has_value()) {
         return;
      }
      auto result = _list_processor.process_cell((*parser_result), _env);
      cell_returned(result);
   } catch (sauros::exceptions::runtime_c &e) {
      except(e);
   } catch (sauros::exceptions::assertion_c &e) {
      except(e);
   } catch (sauros::exceptions::unknown_identifier_c &e) {
      except(e);
   } catch (sauros::parser::parser_exception_c &e) {
      except(e);
   }
}

void driver_if::execute(const char *source, uint64_t line_number,
                        std::string &line) {
   try {
      auto result = _list_processor.process_cell(
          sauros::parser::parse_line(source, line_number, line), _env);
      cell_returned(result);
   } catch (sauros::exceptions::runtime_c &e) {
      except(e);
   } catch (sauros::exceptions::assertion_c &e) {
      except(e);
   } catch (sauros::exceptions::unknown_identifier_c &e) {
      except(e);
   } catch (sauros::parser::parser_exception_c &e) {
      except(e);
   }
}

inline bool blank(std::string &s) {
   if (s.empty() || std::all_of(s.begin(), s.end(),
                                [](char c) { return std::isspace(c); })) {
      return true;
   }
   return false;
}

int file_executor_c::run(const std::string &file) {
   _segment_parser.set_origin(file);

   _file = file;
   _fs.open(_file, std::fstream::in);
   if (!_fs.is_open()) {
      return 1;
   }

   std::string line;
   uint64_t line_number{0};
   while (std::getline(_fs, line)) {
      line_number++;
      execute(parser::segment_parser_c::segment_s{line, line_number});
   }

   _fs.clear();
   indicate_complete();
   _fs.close();
   return 0;
}

void file_executor_c::cell_returned(cell_ptr cell) { /* Not needed */
}

void file_executor_c::except(sauros::parser::parser_exception_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_origin(), e.get_location());
   std::exit(1);
}

void file_executor_c::except(sauros::exceptions::runtime_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_origin(), e.get_location());
   std::exit(1);
}

void file_executor_c::except(sauros::exceptions::assertion_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
   display_error_from_file(e.get_origin(), e.get_location());
   std::exit(1);
}

void file_executor_c::except(sauros::exceptions::unknown_identifier_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << ": "
             << e.get_id() << std::endl;
   display_error_from_file(e.get_origin(), e.get_location());
   std::exit(1);
}

void repl_c::start() {

   std::unordered_map<std::string, std::vector<std::string>> completion_map = {
       {"[u", {"[use "}},
       {"[v", {"[var "}},
       {"[i", {"[import ", "[is_nil ", "[if ", "[iter "}},
       {"[it", {"[iter "}},
       {"[im", {"[import "}},
       {"[is", {"[is_nil "}},
       {"[p", {"[put ", "[putln ", "[push ", "[pop "}},
       {"[c", {"[compose ", "[clear "}},
       {"[co", {"[compose "}},
       {"[comp", {"[compose ", "[decompose "}},
       {"[b",
        {"[box ", "[back ", "[break ", "[bw_and ", "[bw_or ", "[bw_lsh ",
         "[bw_rsh ", "[bw_xor ", "[bw_not "}},
       {"[bw",
        {"[bw_and ", "[bw_or ", "[bw_lsh ", "[bw_rsh ", "[bw_xor ",
         "[bw_not "}},
       {"[bw_",
        {"[bw_and ", "[bw_or ", "[bw_lsh ", "[bw_rsh ", "[bw_xor ",
         "[bw_not "}},
       {"[ba", {"[back "}},
       {"[br", {"[break "}},
       {"[bo", {"[box "}},
       {"[s", {"[set ", "[sneq ", "[seq "}},
       {"[e", {"[exit 0]", "[exit 1]", "[exit "}},
       {"[ni", {"[nil ", "[is_nil "}},
       {"[l", {"[list ", "[len ", "[loop [", "[lambda ["}},
       {"[li", {"[list "}},
       {"[lo", {"[loop ["}},
       {"[la", {"[lambda ["}},
       {"[a", {"[at ", "[assert "}},
       {"[as", {"[assert "}},
       {"[=", {"[== ", "[!= "}},
       {"[!", {"[!= "}},
       {"[>", {"[>= "}},
       {"[<", {"[<= "}},
       {"[x", {"[xor "}},
       {"[t", {"[true ", "[type "}},
       {"[tr", {"[true "}},
       {"[ty", {"[type "}},
   };

   linenoise::SetCompletionCallback(
       [&](const char *editBuffer, std::vector<std::string> &completions) {
          std::string first = editBuffer;
          if (completion_map.find(first) != completion_map.end()) {
             std::vector<std::string> v = completion_map[first];
             completions.insert(completions.end(), v.begin(), v.end());
          }
       });

   std::filesystem::path history_path(".sauros_repl_history.txt");
   {
      system_c system;
      auto home = system.get_sauros_directory();
      if (home.has_value()) {
         history_path = std::filesystem::path(*home);
         history_path /= "repl_history.txt";
      }
   }

   linenoise::SetHistoryMaxLen(repl_c::MAX_HISTORY_LENGTH);
   linenoise::LoadHistory(history_path.c_str());
   linenoise::SetMultiLine(true);

   uint64_t line_number{0};
   bool show_prompt{true};
   std::string buffer;

   std::string prompt = ">>> ";
   while (_do) {

      line_number++;

      if (show_prompt) {
         prompt = ">>> ";
      } else {
         prompt = "";
      }

      std::string line;
      if (linenoise::Readline(prompt.c_str(), line)) {
         _do = false;
         continue;
      }
      if (line.empty()) {
         continue;
      }

      auto buffer = _buffer->submit(line);
      if (buffer.has_value()) {

         execute("repl", line_number, (*buffer));
         show_prompt = true;

         // Add line to history
         linenoise::AddHistory((*buffer).c_str());

         // Save history
         linenoise::SaveHistory(history_path.c_str());

      } else {
         show_prompt = false;
      }
   }
}

void repl_c::stop() { _do = false; }

void repl_c::cell_returned(cell_ptr cell) {

   std::string s_cell;
   _list_processor.cell_to_string(s_cell, cell, _env, true);
   std::cout << s_cell << std::endl;
}

void repl_c::except(sauros::parser::parser_exception_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::except(sauros::exceptions::runtime_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::except(sauros::exceptions::assertion_c &e) {
   std::cout << rang::fg::red << e.what() << rang::fg::reset << std::endl;
}

void repl_c::except(sauros::exceptions::unknown_identifier_c &e) {

   std::cout << rang::fg::red << e.what() << rang::fg::reset << ": "
             << e.get_id() << std::endl;
}

void eval_c::cell_returned(cell_ptr cell) { _cb(cell); }

void eval_c::except(sauros::parser::parser_exception_c &e) {
   std::cout << rang::fg::yellow << "[decomposed item] : " << rang::fg::red
             << e.what() << rang::fg::reset << std::endl;
   std::exit(1);
}

void eval_c::except(sauros::exceptions::runtime_c &e) {
   std::cout << rang::fg::yellow << "[decomposed item] : " << rang::fg::red
             << e.what() << rang::fg::reset << std::endl;
   std::exit(1);
}

void eval_c::except(sauros::exceptions::assertion_c &e) {
   std::cout << rang::fg::yellow << "[decomposed item] : " << rang::fg::red
             << e.what() << rang::fg::reset << std::endl;
   std::exit(1);
}

void eval_c::except(sauros::exceptions::unknown_identifier_c &e) {
   std::cout << rang::fg::yellow << "[decomposed item] : " << rang::fg::red
             << e.what() << rang::fg::reset << ": " << e.get_id() << std::endl;
   std::exit(1);
}

} // namespace sauros