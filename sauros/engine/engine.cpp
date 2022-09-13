#include "engine.hpp"

#include <iostream>
namespace sauros {

engine_c::engine_c() {

   _cell_loader = std::make_shared<engine_c::engine_loader_c>(this);
}

engine_c::result_s engine_c::process(std::vector<std::shared_ptr<list_c>>& lists) {



}

engine_c::result_s engine_c::run_list(std::shared_ptr<list_c>& list) {

   auto& target_list = list.get()->cells;

   // iterate in reverse to work with stack
   std::vector<std::shared_ptr<cell_c>>::reverse_iterator it = target_list.rbegin();

   auto loader = _cell_loader.get();

   while (it != target_list.rend()) {
      (*it).get()->visit(*loader);
      it++;
   }
}

void engine_c::engine_loader_c::accept(symbol_c &cell) {

   std::cout << "SYMBOL: " << cell.data << std::endl;
}

void engine_c::engine_loader_c::accept(list_c &cell) {

}

void engine_c::engine_loader_c::accept(string_c &cell) {

}

void engine_c::engine_loader_c::accept(integer_c &cell) {

}

void engine_c::engine_loader_c::accept(double_c &cell) {

}


} // namespace sauros