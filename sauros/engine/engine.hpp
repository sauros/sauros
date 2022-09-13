#ifndef SAUROS_ENGINE_HPP
#define SAUROS_ENGIONE_HPP

#include "sauros/types.hpp"
#include "sauros/list.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

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
      env_exception_c(const char* message) : message(message){}
      const char * what () const throw () {
         return message;
      }
      const char* message {nullptr};
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

    private:
      std::unordered_map<std::string, std::shared_ptr<cell_c>> _data;
      std::shared_ptr<env_c> _outer;
   };

   enum class operations_e {
      ADD, SUB, DIV, MOD, SET, LOAD
   };

   struct operation_s {
      operations_e op;
      std::shared_ptr<cell_c> cell;
   };

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
};

} // namespace sauros

#endif