#include "random.hpp"

#include "sauros/processor/processor.hpp"
#include <string>
#include <random>

namespace sauros {
namespace modules {

random_c::random_c() {
   _members_map["uniform_int"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int function expects 2 parameters (min, max), but " +
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
          } catch(...) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int failed to convert parameters to integers (stoi)",
                 cells[2].location);
          }

          if (min_int >= max_int) {
             throw processor_c::runtime_exception_c(
                 "random::uniform_int minimum value must be less than maximum value",
                 cells[2].location);
          }
          
          std::random_device rd;
          std::mt19937 mt(rd());
          std::uniform_int_distribution<int> dist(min_int, max_int);

          return {cell_c(cell_type_e::INTEGER, std::to_string(dist(mt)))};
       });
}

} // namespace modules
} // namespace sauros