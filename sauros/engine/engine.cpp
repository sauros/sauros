#include "engine.hpp"

#include <iostream>
namespace sauros {

engine_c::engine_c() {

   _cell_loader = std::make_shared<engine_c::engine_loader_c>(this);
   _env = std::make_shared<env_c>();

   populate_function_map();
}

engine_c::result_s engine_c::process(std::vector<std::shared_ptr<list_c>>& lists) {

   for (auto& list : lists) {
      auto result = run_list(list);

      if (result.code != result_code_e::OKAY) {
         return result;
      }
   }

   result_s result;
   result.code = result_code_e::OKAY;
   return result;
}

engine_c::result_s engine_c::run_list(std::shared_ptr<list_c>& list) {

   result_s result;
   result.code = result_code_e::OKAY;

   auto& target_list = list.get()->cells;

   // iterate in reverse to work with stack
   std::vector<std::shared_ptr<cell_c>>::reverse_iterator it = target_list.rbegin();

   auto loader = _cell_loader.get();

   while (it != target_list.rend()) {
      (*it).get()->visit(*loader);
      it++;
   }

   return result;
}

void engine_c::engine_loader_c::accept(list_c &cell) {

   auto list = std::make_shared<list_c>(cell);
   _engine->run_list(list);
}

void engine_c::engine_loader_c::accept(string_c &cell) {

   std::cout << "STRING: " << cell.data << std::endl;
   
   _engine->_stack.push({
      .type = data_type_e::STRING,
      .data = cell.data
   });
}

void engine_c::engine_loader_c::accept(integer_c &cell) {

   std::cout << "INTEGER: " << cell.data << std::endl;
   
   _engine->_stack.push({
      .type = data_type_e::INTEGER,
      .data = cell.data
   });
}

void engine_c::engine_loader_c::accept(double_c &cell) {

   std::cout << "DOUBLE: " << cell.data << std::endl;

   _engine->_stack.push({
      .type = data_type_e::DOUBLE,
      .data = cell.data
   });
}

void engine_c::engine_loader_c::accept(symbol_c &cell) {

   std::cout << "SYMBOL: " << cell.data << std::endl;

   // Check to see if the item is a mapped function
   //
   if (_engine->_function_map.find(cell.data) != _engine->_function_map.end()) {

      auto rhs = _engine->_stack.top();
      _engine->_stack.pop();

      auto lhs = _engine->_stack.top();
      _engine->_stack.pop();

      _engine->_function_map[cell.data](lhs, rhs);
      return;
   }

   // Push whatever it is to the stack as it an identifier
   //
   _engine->_stack.push({
      .type = data_type_e::IDENTIFIER,
      .data = cell.data
   });
   return;
}

std::shared_ptr<cell_c> engine_c::load_variable_copy(const std::string& target_var) {
   try {
      auto env = _env.get()->find(target_var);
      auto value = env[target_var];
      std::shared_ptr<cell_c> cell(value);
      return std::move(cell);
   }
   catch (env_exception_c& e) {
      throw run_time_exception_c(e.what());
   }
}

void engine_c::populate_function_map() {

   _function_map["-"] = [](const operation_s& lhs, const operation_s& rhs) { std::cout << "Not complete\n"; return; };
   _function_map["/"] = [](const operation_s& lhs, const operation_s& rhs) { std::cout << "Not complete\n"; return; };
   _function_map["*"] = [](const operation_s& lhs, const operation_s& rhs) { std::cout << "Not complete\n"; return; };
   _function_map["%"] = [](const operation_s& lhs, const operation_s& rhs) { std::cout << "Not complete\n"; return; };

   _function_map["var"] = [this](const operation_s& lhs, const operation_s& rhs) {
      if (rhs.type != data_type_e::IDENTIFIER) {
         std::cerr << "Expected identifier" << std::endl;
      }
      std::string varible_name = std::get<std::string>(rhs.data);

      switch (lhs.type) {
         case data_type_e::DOUBLE: { 
            std::shared_ptr<cell_c> cell (new double_c(get<double>(lhs.data), {0,0})); 
               _env.get()->get(varible_name) = std::move(cell);
            break; 
         }
         case data_type_e::INTEGER: {
            std::shared_ptr<cell_c> cell (new integer_c(get<int64_t>(lhs.data), {0,0})); 
               _env.get()->get(varible_name) = std::move(cell);
            break; 
         }
         case data_type_e::STRING:  {
            std::shared_ptr<cell_c> cell (new string_c(get<std::string>(lhs.data), {0,0})); 
               _env.get()->get(varible_name) = std::move(cell);
            break; 
         }
         case data_type_e::IDENTIFIER: {

            // Retrieve a copy of the value that the IDENTIFIER contains
            //
            auto cell = load_variable_copy(std::get<std::string>(lhs.data));
            _env.get()->get(varible_name) = std::move(cell);
            break;
         }
      }
   };


   // ADD
   //
   _function_map["+"] = [this](const operation_s& lhs, const operation_s& rhs)  {
      switch (lhs.type) {
         case data_type_e::DOUBLE: 
         {
            switch (rhs.type) {
               case data_type_e::DOUBLE: 
               {
                   _stack.push(operation_s {
                     .type = data_type_e::DOUBLE,
                     .data = std::get<double>(lhs.data) + std::get<double>(rhs.data)
                  });
               };
               case data_type_e::INTEGER: 
               {
                   _stack.push(operation_s {
                     .type = data_type_e::DOUBLE,
                     .data = std::get<double>(lhs.data) + std::get<int64_t>(rhs.data)
                  });
               };
               case data_type_e::STRING: 
               {
                  throw run_time_exception_c("Unable to perform operation on types  DOUBLE and STRING");
                  break;
               };
               case data_type_e::IDENTIFIER: 
               {
                  break;
               };
            }
            break;
         };
         case data_type_e::INTEGER: 
         {
            switch (rhs.type) {
               case data_type_e::DOUBLE: 
               {
                   _stack.push(operation_s {
                     .type = data_type_e::DOUBLE,
                     .data = std::get<double>(lhs.data) + std::get<double>(rhs.data)
                  });
               };
               case data_type_e::INTEGER: 
               {
                   _stack.push(operation_s {
                     .type = data_type_e::INTEGER,
                     .data = std::get<int64_t>(lhs.data) + std::get<int64_t>(rhs.data)
                  });
               };
               case data_type_e::STRING: 
               {
                  break;
               };
               case data_type_e::IDENTIFIER: 
               {
                  break;
               };
            }
            break;
         };
         case data_type_e::STRING: 
         {
            break;
         };
         case data_type_e::IDENTIFIER: 
         {
            break;
         };
      }
   };
}


} // namespace sauros