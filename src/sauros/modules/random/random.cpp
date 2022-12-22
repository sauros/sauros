#include "random.hpp"

#include "sauros/processor/processor.hpp"
#include <random>
#include <string>

namespace sauros {
namespace modules {

namespace {
static constexpr char ALL_CHARS[] = "0123456789"
                                    "~!@#$%^&*()_+"
                                    "`-=<>,./?\"';:"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

static constexpr char ALPHA_NUM[] = "0123456789"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

std::string generate_random_string(const std::string source, std::size_t len) {
   std::random_device rd;
   std::mt19937 mt(rd());
   std::uniform_int_distribution<std::size_t> dist(0, source.size() - 1);
   std::string result;

   if (source.empty()) {
      return {};
   }

   for (auto i = 0; i < len; i++) {
      result += source.at(dist(mt));
   }
   return result;
}
} // namespace

random_c::random_c() {

   _members_map["string"] =
       cell_c([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "random::string function expects 1 parameters (length), but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto len = load(cells[1], env);
          if (len.type != cell_type_e::DOUBLE &&
              len.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::string expects parameter `len` to be numerical",
                 cells[1].location);
          }

          int len_int = 0;
          try {
             len_int = std::stoi(len.data);
          } catch (...) {
             throw processor_c::runtime_exception_c(
                 "random::string failed to convert parameter to integer (stoi)",
                 cells[1].location);
          }
          return cell_c(cell_type_e::STRING,
                        generate_random_string(ALL_CHARS, len_int));
       });

   _members_map["alpha_string"] =
       cell_c([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "random::string function expects 1 parameters (length), but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto len = load(cells[1], env);
          if (len.type != cell_type_e::DOUBLE &&
              len.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::string expects parameter `len` to be numerical",
                 cells[1].location);
          }

          int len_int = 0;
          try {
             len_int = std::stoi(len.data);
          } catch (...) {
             throw processor_c::runtime_exception_c(
                 "random::string failed to convert parameter to integer (stoi)",
                 cells[1].location);
          }
          return cell_c(cell_type_e::STRING,
                        generate_random_string(ALPHA_NUM, len_int));
       });

   _members_map["sourced_string"] = cell_c([this](cells_t &cells,
                                                  std::shared_ptr<environment_c>
                                                      env) -> cell_c {
      if (cells.size() != 3) {
         throw processor_c::runtime_exception_c(
             "random::string function expects 2 parameters (source_string, "
             "length), but " +
                 std::to_string(cells.size() - 1) + " were given",
             cells[0].location);
      }

      auto src = load(cells[1], env);
      if (src.type != cell_type_e::STRING) {
         throw processor_c::runtime_exception_c(
             "random::string expects parameter `source_string` to be a string",
             cells[1].location);
      }

      auto len = load(cells[2], env);
      if (len.type != cell_type_e::DOUBLE && len.type != cell_type_e::INTEGER) {
         throw processor_c::runtime_exception_c(
             "random::string expects parameter `len` to be numerical",
             cells[2].location);
      }

      int len_int = 0;
      try {
         len_int = std::stoi(len.data);
      } catch (...) {
         throw processor_c::runtime_exception_c(
             "random::string failed to convert parameter to integer (stoi)",
             cells[2].location);
      }
      return cell_c(cell_type_e::STRING,
                    generate_random_string(src.data, len_int));
   });

   _members_map["uniform_int"] =
       cell_c([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int function expects 2 parameters (min, "
                 "max), but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto min = load(cells[1], env);
          if (min.type != cell_type_e::DOUBLE &&
              min.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int expects parameter `min` to be numerical",
                 cells[1].location);
          }

          auto max = load(cells[2], env);
          if (max.type != cell_type_e::DOUBLE &&
              max.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int expects parameter `max` to be numerical",
                 cells[2].location);
          }

          int min_int = 0;
          int max_int = 0;
          try {
             max_int = std::stoi(max.data);
             min_int = std::stoi(min.data);
          } catch (...) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int failed to convert parameters to integers "
                 "(stoi)",
                 cells[2].location);
          }

          if (min_int >= max_int) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int minimum value must be less than maximum "
                 "value",
                 cells[2].location);
          }

          std::random_device rd;
          std::mt19937 mt(rd());
          std::uniform_int_distribution<int> dist(min_int, max_int);

          return cell_c(cell_type_e::INTEGER, std::to_string(dist(mt)));
       });

   _members_map["uniform_real"] =
       cell_c([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_real function expects 2 parameters (min, "
                 "max), but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto min = load(cells[1], env);
          if (min.type != cell_type_e::DOUBLE &&
              min.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_real expects parameter `min` to be numerical",
                 cells[1].location);
          }

          auto max = load(cells[2], env);
          if (max.type != cell_type_e::DOUBLE &&
              max.type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_real expects parameter `max` to be numerical",
                 cells[2].location);
          }

          double min_int = 0;
          double max_int = 0;
          try {
             max_int = std::stod(max.data);
             min_int = std::stod(min.data);
          } catch (...) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_real failed to convert parameters to "
                 "integers (stoi)",
                 cells[2].location);
          }

          if (min_int >= max_int) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_real minimum value must be less than maximum "
                 "value",
                 cells[2].location);
          }

          std::random_device rd;
          std::mt19937 mt(rd());
          std::uniform_real_distribution<double> dist(min_int, max_int);

          return cell_c(cell_type_e::DOUBLE, std::to_string(dist(mt)));
       });
}

} // namespace modules
} // namespace sauros