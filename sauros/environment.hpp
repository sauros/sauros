#ifndef SAUROS_ENVIRONMENT_HPP
#define SAUROS_ENVIRONMENT_HPP

#include "sauros/list.hpp"
#include <unordered_map>
#include <vector>

namespace sauros {

class environment_c {
public:

   class unknown_identifier_c : public std::exception {
   public:
      unknown_identifier_c() = delete;
      unknown_identifier_c(std::string identifier) : _id(identifier) {}
      const char* what() const throw () {
         return "Unknown identifier";
      }
      std::string get_id() const { return _id; }
   private:
      std::string _id;
   };

   environment_c(std::shared_ptr<environment_c> outer = nullptr) : _parent(outer){}
   environment_c(std::vector<cell_c>& params, std::vector<cell_c>& args, std::shared_ptr<environment_c> outer); 

   bool exists(const std::string& item);

   void set(const std::string& item, cell_c cell);
   cell_c get(const std::string& item);

   environment_c* find(const std::string& item);

private:
   std::shared_ptr<environment_c> _parent{nullptr};
   std::unordered_map<std::string, cell_c> _env;
};

} // namespace sauros

#endif