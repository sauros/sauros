#ifndef SAUROS_ENGINE_HPP
#define SAUROS_ENGIONE_HPP

#include "sauros/types.hpp"
#include "sauros/list.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>
#include <variant>

namespace sauros {

class engine_c {
public:

   enum class result_code_e {
      OKAY,
      INVALID_OPERATION
   };

   struct result_s {
      result_code_e code {result_code_e::OKAY};
      location_s location {0,0};
      std::string message;
   };

   //! \brief Construct the engine
   engine_c();

   //! \brief Process a series of lists
   //! \returns a result_s structure detailing the result of the
   //!          processing
   result_s process(std::vector<std::shared_ptr<list_c>>& lists);

private: 

   // Process a single list
   // 
   result_s run_list(std::shared_ptr<list_c>& list);

   // An exception that can be thrown when trying to access something
   // from the environment
   //
   struct env_exception_c : public std::exception {
      env_exception_c(std::string message) : message(message){}
      const char * what () const throw () {
         return message.c_str();
      }
      std::string message;
   };

   struct run_time_exception_c : public std::exception {
      run_time_exception_c(std::string message) : message(message){}
      const char * what () const throw () {
         return message.c_str();
      }
      std::string message;
   };

   // The data environment
   //
   class env_c {
    public:
      env_c() {}
      env_c(std::shared_ptr<env_c> outer): _outer(outer) {}

      std::shared_ptr<cell_c> &operator[](const std::string& item) { return _data[item]; }
      std::shared_ptr<cell_c> &get(const std::string& item) { return _data[item]; }

      std::unordered_map<std::string, std::shared_ptr<cell_c>> &find(const std::string &var) {
         if (_data.find(var) != _data.end()) {
            return _data;
         }
         if (_outer) {
            return _outer->find(var);
         }

         std::string err = "Unbound symbol : [" + var + "]";
         throw env_exception_c(err.c_str());
      }

      std::unordered_map<std::string, std::shared_ptr<cell_c>> get_map() { return _data; }

    private:
      std::unordered_map<std::string, std::shared_ptr<cell_c>> _data;
      std::shared_ptr<env_c> _outer;
   };

   enum class data_type_e {
      INTEGER, DOUBLE, STRING, IDENTIFIER
   };

   struct operation_s {
      data_type_e type;
      std::variant<int64_t, double, std::string> data;
   };

   using operand_f = std::function<void(const operation_s& lhs, const operation_s& rhs)>;

   // The visitor class 
   //
   class engine_loader_c : public cell_visitor_c {
    public:
      engine_loader_c(engine_c* engine) { _engine = engine; }
      virtual void accept(symbol_c &cell) override final;
      virtual void accept(list_c &cell) override final;
      virtual void accept(string_c &cell) override final;
      virtual void accept(integer_c &cell) override final;
      virtual void accept(double_c &cell) override final;

    private:
      engine_c* _engine{nullptr};
   };

   std::shared_ptr<engine_loader_c> _cell_loader;
   std::shared_ptr<env_c> _env;

   std::stack<operation_s> _stack;
   std::unordered_map<std::string, operand_f> _function_map;

   std::shared_ptr<cell_c> load_variable_copy(const std::string& name);
   void populate_function_map();
};

} // namespace sauros

#endif