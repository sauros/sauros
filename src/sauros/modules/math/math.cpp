#include "math.hpp"

#include <iostream>
#include <string>
#include <math.h> 

#include "sauros/processor/processor.hpp"

namespace sauros {
namespace modules {

math_c::math_c() {

   auto load = [&](cell_c &cell, std::shared_ptr<environment_c> env) -> cell_c {
      processor_c processor;


      auto target = processor.process_cell(cell, env);
      if (!target.has_value()) {
         throw processor_c::runtime_exception_c("Unable to process value", cell.location);
      }
      return (*target);
   };

   _members_map["pi"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "math::pi expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
         }

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "3.14159265359")};
   });

   _members_map["e"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "math::e expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
         }

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "2.71828182845")};
   });

   _members_map["phi"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "math::phi expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
         }

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "1.61803398875")};
   });

   _members_map["log"] = cell_c(
       [this, load](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          if (cells.size() == 1) {
              throw processor_c::runtime_exception_c(
                  "math::log expects 1 parameter, but " +
                      std::to_string(cells.size() - 1) + " were given",
                  cells[0].location);
          }

          auto op = [load](cell_c &cell, std::shared_ptr<environment_c> env) -> double {
               auto item = load(cell, env);
               if (item.type != cell_type_e::DOUBLE && item.type != cell_type_e::INTEGER) {
                  throw processor_c::runtime_exception_c(
                        "math::log expects parameter to be an integer or a double",
                        cell.location);
               }

               return log(std::stod(item.data));
          };

          if (cells.size() == 2) {
            return cell_c(cell_type_e::DOUBLE, std::to_string(op(cells[1], env)));
          }

          cell_c result(cell_type_e::LIST);
          for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
            result.list.push_back(cell_c(cell_type_e::DOUBLE, std::to_string(op((*c), env))));
          }
          return result;
   });

   _members_map["log10"] = cell_c(
       [this, load](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          if (cells.size() == 1) {
              throw processor_c::runtime_exception_c(
                  "math::log expects 1 parameter, but " +
                      std::to_string(cells.size() - 1) + " were given",
                  cells[0].location);
          }

          auto op = [load](cell_c &cell, std::shared_ptr<environment_c> env) -> double {
               auto item = load(cell, env);
               if (item.type != cell_type_e::DOUBLE && item.type != cell_type_e::INTEGER) {
                  throw processor_c::runtime_exception_c(
                        "math::log10 expects parameter to be an integer or a double",
                        cell.location);
               }

               return log10(std::stod(item.data));
          };

          if (cells.size() == 2) {
            return cell_c(cell_type_e::DOUBLE, std::to_string(op(cells[1], env)));
          }

          cell_c result(cell_type_e::LIST);
          for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
            result.list.push_back(cell_c(cell_type_e::DOUBLE, std::to_string(op((*c), env))));
          }
          return result;
   });
}



} // namespace modules
} // namespace sauros