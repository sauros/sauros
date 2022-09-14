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
      std::optional<cell_c> returned_value;
   };

   processor_c();

   result_s process(cell_c& cell, std::shared_ptr<environment_c> global_env);

   void cell_to_string(std::string& out, cell_c& cell, std::shared_ptr<environment_c> env, bool show_space = true);

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

   std::unordered_map<std::string, cell_c> _builtins;

   void populate_builtins();

   std::optional<cell_c>  process_list(std::vector<cell_c>& cells, std::shared_ptr<environment_c> env);
   std::optional<cell_c>  process_cell(cell_c& cell, std::shared_ptr<environment_c> env);
   
   cell_c perform_arithmetic(std::string op, std::vector<cell_c>& cells, std::function<double(double, double)>fn, std::shared_ptr<environment_c> env, bool force_double = false);
};

} // namespace sauros

#endif