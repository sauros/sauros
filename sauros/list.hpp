#ifndef SAUROS_LIST_HPP
#define SAUROS_LIST_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "types.hpp"

namespace sauros {

enum class cell_type_e {
   SYMBOL, LIST, STRING, INTEGER, DOUBLE
};

class symbol_c;
class list_c;
class string_c;
class integer_c;
class double_c;

class cell_visitor_c {
public:
   virtual void accept(symbol_c &cell) = 0;
   virtual void accept(list_c &cell) = 0;
   virtual void accept(string_c &cell) = 0;
   virtual void accept(integer_c &cell) = 0;
   virtual void accept(double_c &cell) = 0;
};

class cell_c {
public:
   cell_c(cell_type_e type, location_s location) : type(type), location(location){}
   virtual void visit(cell_visitor_c &visitor) = 0;
   cell_type_e type;
   location_s location;
};

class symbol_c : public cell_c {
public:
   symbol_c(std::string data, location_s locationa) : cell_c(cell_type_e::SYMBOL, location), data(data) {}
   std::string data;

   virtual void visit(cell_visitor_c &visitor) override final {
      visitor.accept(*this);
   }
};

class list_c : public cell_c {
public:
   list_c(location_s location) : cell_c(cell_type_e::LIST, location) {}
   list_c(std::vector<std::shared_ptr<cell_c>> cells, location_s location) : cell_c(cell_type_e::LIST, location), cells(std::move(cells)) {}
   std::vector<std::shared_ptr<cell_c>> cells;

   virtual void visit(cell_visitor_c &visitor) override final {
      visitor.accept(*this);
   }
};

class string_c : public cell_c {
public:
   string_c(std::string data, location_s location) : cell_c(cell_type_e::STRING, location), data(data) {}
   std::string data;

   virtual void visit(cell_visitor_c &visitor) override final {
      visitor.accept(*this);
   }
};

class integer_c : public cell_c {
public:
   integer_c(int64_t data, location_s location) : cell_c(cell_type_e::INTEGER, location), data(data) {}
   int64_t data;

   virtual void visit(cell_visitor_c &visitor) override final {
      visitor.accept(*this);
   }
};

class double_c : public cell_c {
public:
   double_c(double data, location_s location) : cell_c(cell_type_e::DOUBLE, location), data(data) {}
   double data;

   virtual void visit(cell_visitor_c &visitor) override final {
      visitor.accept(*this);
   }
};






} // namespace sauros

#endif