#ifndef SAUROS_PROCESSOR_HPP
#define SAUROS_PROCESSOR_HPP

#include "sauros/list.hpp"
#include "sauros/environment.hpp"

#include <unordered_map>
#include <functional>
#include <optional>
#include <vector>
#include <exception>

namespace sauros {


class processor_c {
public:

   enum class result_code_e {
      OKAY
   };

   struct result_s {
      result_code_e code{result_code_e::OKAY};

   };

   processor_c(std::shared_ptr<environment_c> global_env);

   result_s process(cell_c& cell);

   class runtime_exception_c : public std::exception {
   public:
      runtime_exception_c() = delete;
      runtime_exception_c(std::string message, location_s location) : _msg(message), _loc(location) {}
      const char* what() const throw () {
         return _msg.c_str();
      }
      const location_s get_location() { return _loc; }
   private:
      std::string _msg;
      location_s _loc{0,0};
   };


private:

   std::shared_ptr<environment_c> _global_env;
   std::unordered_map<std::string, cell_c> _builtins;

   void populate_builtins();

   std::optional<cell_c>  process_list(std::vector<cell_c>& cells);
   std::optional<cell_c>  process_cell(cell_c& cell);

   std::optional<cell_c> fn_define_variable(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_put(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_make_lambda(std::vector<cell_c>& cells);

   cell_c perform_arithmetic(std::string op, std::vector<cell_c>& cells, std::function<double(double, double)>fn, bool force_double = false);
   std::optional<cell_c> fn_add(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_sub(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_div(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_mul(std::vector<cell_c>& cells);
   std::optional<cell_c> fn_mod(std::vector<cell_c>& cells);

};

} // namespace sauros

#endif