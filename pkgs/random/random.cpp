#include "random.hpp"

#include <random>
#include <sauros/capi/capi.hpp>
#include <string>

static constexpr char ALL_CHARS[] = "0123456789"
                                    "~!@#$%^&*()_+"
                                    "`-=<>,./?\"';:"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

static constexpr char ALPHA_NUM[] = "0123456789"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

static std::string generate_random_string(const std::string source,
                                          std::size_t len) {
   std::random_device rd;
   std::mt19937 mt(rd());
   std::uniform_int_distribution<std::size_t> dist(0, source.size() - 1);
   std::string result;

   if (source.empty()) {
      return {};
   }

   for (auto i = 0; i < len; i++) {
      result += source.at(dist(mt));
   }
   return result;
}

sauros::cell_ptr
_pkg_random_string_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 2) {
      throw sauros::processor_c::runtime_exception_c(
          "random::string function expects 1 parameters (length), but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]->location);
   }

   auto len = c_api_process_cell(cells[1], env);
   if (len->type != sauros::cell_type_e::DOUBLE &&
       len->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::string expects parameter `len` to be numerical",
          cells[1]->location);
   }

   int len_int = 0;
   try {
      len_int = std::stoi(len->data);
   } catch (...) {
      throw sauros::processor_c::runtime_exception_c(
          "random::string failed to convert parameter to integer (stoi)",
          cells[1]->location);
   }
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::STRING, generate_random_string(ALL_CHARS, len_int));
}

sauros::cell_ptr
_pkg_random_alpha_string_(sauros::cells_t &cells,
                          std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 2) {
      throw sauros::processor_c::runtime_exception_c(
          "random::alpha_string function expects 1 parameters (length), but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]->location);
   }

   auto len = c_api_process_cell(cells[1], env);
   if (len->type != sauros::cell_type_e::DOUBLE &&
       len->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::alpha_string expects parameter `len` to be numerical",
          cells[1]->location);
   }

   int len_int = 0;
   try {
      len_int = std::stoi(len->data);
   } catch (...) {
      throw sauros::processor_c::runtime_exception_c(
          "random::alpha_string failed to convert parameter to integer (stoi)",
          cells[1]->location);
   }
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::STRING, generate_random_string(ALPHA_NUM, len_int));
}

sauros::cell_ptr
_pkg_random_sourced_string_(sauros::cells_t &cells,
                            std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 3) {
      throw sauros::processor_c::runtime_exception_c(
          "random::sourced_string function expects 2 parameters "
          "(source_string, "
          "length), but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]->location);
   }

   auto src = c_api_process_cell(cells[1], env);
   if (src->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "random::sourced_string expects parameter `source_string` to be a "
          "string",
          cells[1]->location);
   }

   auto len = c_api_process_cell(cells[2], env);
   if (len->type != sauros::cell_type_e::DOUBLE &&
       len->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::sourced_string expects parameter `len` to be numerical",
          cells[2]->location);
   }

   int len_int = 0;
   try {
      len_int = std::stoi(len->data);
   } catch (...) {
      throw sauros::processor_c::runtime_exception_c(
          "random::sourced_string failed to convert parameter to integer "
          "(stoi)",
          cells[2]->location);
   }
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::STRING, generate_random_string(src->data, len_int));
}

sauros::cell_ptr
_pkg_random_uniform_int_(sauros::cells_t &cells,
                         std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 3) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_int function expects 2 parameters (min, "
          "max), but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]->location);
   }

   auto min = c_api_process_cell(cells[1], env);
   if (min->type != sauros::cell_type_e::DOUBLE &&
       min->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_int expects parameter `min` to be numerical",
          cells[1]->location);
   }

   auto max = c_api_process_cell(cells[2], env);
   if (max->type != sauros::cell_type_e::DOUBLE &&
       max->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_int expects parameter `max` to be numerical",
          cells[2]->location);
   }

   int min_int = 0;
   int max_int = 0;
   try {
      max_int = std::stoi(max->data);
      min_int = std::stoi(min->data);
   } catch (...) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_int failed to convert parameters to integers "
          "(stoi)",
          cells[2]->location);
   }

   if (min_int >= max_int) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_int minimum value must be less than maximum "
          "value",
          cells[2]->location);
   }

   std::random_device rd;
   std::mt19937 mt(rd());
   std::uniform_int_distribution<int> dist(min_int, max_int);

   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,
                                           std::to_string(dist(mt)));
}

sauros::cell_ptr
_pkg_random_uniform_real_(sauros::cells_t &cells,
                          std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 3) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_real function expects 2 parameters (min, "
          "max), but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]->location);
   }

   auto min = c_api_process_cell(cells[1], env);
   if (min->type != sauros::cell_type_e::DOUBLE &&
       min->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_real expects parameter `min` to be numerical",
          cells[1]->location);
   }

   auto max = c_api_process_cell(cells[2], env);
   if (max->type != sauros::cell_type_e::DOUBLE &&
       max->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_real expects parameter `max` to be numerical",
          cells[2]->location);
   }

   double min_int = 0;
   double max_int = 0;
   try {
      max_int = std::stod(max->data);
      min_int = std::stod(min->data);
   } catch (...) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_real failed to convert parameters to "
          "integers (stoi)",
          cells[2]->location);
   }

   if (min_int >= max_int) {
      throw sauros::processor_c::runtime_exception_c(
          "random::uniform_real minimum value must be less than maximum "
          "value",
          cells[2]->location);
   }

   std::random_device rd;
   std::mt19937 mt(rd());
   std::uniform_real_distribution<double> dist(min_int, max_int);

   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::DOUBLE,
                                           std::to_string(dist(mt)));
}
