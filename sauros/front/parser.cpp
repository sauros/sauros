#include "parser.hpp"

#include <iostream>
#include <regex>
#include <tuple>
#include <vector>

namespace sauros {
namespace parser {

enum class token_e {
   L_BRACKET,
   R_BRACKET,
   SYMBOL,
   INTEGER,
   DOUBLE,
   STRING,
};

struct token_s {
   token_e token;
   std::string data;
   location_s location;
};

static std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

static bool is_digit(const char c) {
   return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

//    Retrieve a list of tokens based on the given string
//
std::tuple<std::vector<token_s>, std::shared_ptr<error::error_c>>
tokenize(size_t line_number, const std::string line) {

   std::vector<token_s> tokens;
   for (size_t idx = 0; idx < line.size(); idx++) {
      auto current = line[idx];

      if (std::isspace(current)) {
         continue;
      }

      std::string current_data;
      switch (current) {
      case '[': {
         tokens.push_back({token_e::L_BRACKET, "[", {line_number, idx}});
         continue;
      }

      case ']': {
         tokens.push_back({token_e::R_BRACKET, "]", {line_number, idx}});
         continue;
      }

      case '"': {
         bool in_str{false};
         std::string value;
         decltype(idx) start = idx;
         while (idx < line.size()) {
            if (line[idx] == '"') {
               if (idx > 0 && line[idx - 1] != '\\') {
                  in_str = !in_str;
               }
            }
            if (in_str) {
               value += line[idx];
            } else if (!std::isspace(line[idx]) && line[idx] != '[' &&
                       line[idx] != ']') {
               value += line[idx];
            } else {
               --idx;
               break;
            }
            ++idx;
         }

         if (!value.ends_with('"')) {

            auto error = std::make_shared<error::error_c>(
                location_s(line_number, idx), line.c_str(),
                "Unterminated string");

            return std::make_tuple(tokens, error);
         }

         tokens.push_back({token_e::STRING,
                           value.substr(1, value.size() - 2),
                           {line_number, start}});
         continue;
      }

      default:
         break;
      };

      // Check for an integer / double
      //
      if (is_digit(line[idx])) {

         decltype(idx) start = idx;
         while (is_digit(line[idx]) || line[idx] == '.') {
            current_data += line[idx];
            idx++;
         }
         idx--;

         if (std::regex_match(current_data, is_number)) {
            if (current_data.find('.') != std::string::npos) {
               tokens.push_back(
                   {token_e::DOUBLE, current_data, {line_number, start}});
               continue;
            } else {
               tokens.push_back(
                   {token_e::INTEGER, current_data, {line_number, start}});
               continue;
            }
         } else {
            // Some malformed oddity like: 4444.2323.232.4.34.
            auto error = std::make_shared<error::error_c>(
                location_s(line_number, start), line.c_str(),
                "Malformed representation of suspected numerical");

            return std::make_tuple(tokens, error);
         }
      }

      // Anything else should be considered a symbol
      //
      std::string value;
      decltype(idx) start = idx;
      while (idx < line.size() && !std::isspace(line[idx]) &&
             line[idx] != '[' && line[idx] != ']') {
         value += line[idx];
         idx++;
      }

      tokens.push_back({token_e::SYMBOL, value, {line_number, start}});

      idx--;
   }
   return std::make_tuple(tokens, nullptr);
}

namespace {
std::shared_ptr<error::error_c> get_no_list_error(token_s current_token) {
   return std::make_shared<error::error_c>(
       current_token.location, current_token.data,
       "attempting to create object prior to list creation");
}
} // namespace

std::tuple<cell_c, std::shared_ptr<error::error_c>>
parse(std::vector<token_s> &tokens, cell_c *current_list = nullptr) {

   if (tokens.empty()) {
      return std::make_tuple(cell_c(), nullptr);
   }

   auto current_token = tokens[0];
   tokens = std::vector<token_s>(tokens.begin() + 1, tokens.end());

   switch (current_token.token) {

   case token_e::L_BRACKET: {
      cell_c new_list;
      new_list.type = cell_type_e::LIST;

      // Populate the list
      auto [_, err] = parse(tokens, &new_list);

      if (err) {
         return std::make_tuple(cell_c(), err);
      }

      // If we had  a list, we add our new list to it
      if (current_list) {
         current_list->list.push_back(new_list);

         return parse(tokens, current_list);

         // otherwise we return the new list
      } else {
         return std::make_tuple(new_list, nullptr);
      }
   }

   case token_e::R_BRACKET: {
      // This means we are done building whatever current_list is
      if (!current_list) {
         auto error = std::make_shared<error::error_c>(
             current_token.location, current_token.data,
             "unopened closing bracket `]` located");
         return std::make_tuple(cell_c(), error);
      }

      // The cell list being created has been populated recursively so
      // we don't need to return anything here.
      return std::make_tuple(cell_c(), nullptr);
   }

   case token_e::SYMBOL: {
      if (!current_list) {
         return std::make_tuple(cell_c(), get_no_list_error(current_token));
      }

      current_list->list.push_back(cell_c(
          cell_type_e::SYMBOL, current_token.data, current_token.location));
      return parse(tokens, current_list);
   }

   case token_e::STRING: {
      if (!current_list) {
         return std::make_tuple(cell_c(), get_no_list_error(current_token));
      }

      current_list->list.push_back(cell_c(
          cell_type_e::STRING, current_token.data, current_token.location));
      return parse(tokens, current_list);
   }

   case token_e::INTEGER: {
      if (!current_list) {
         return std::make_tuple(cell_c(), get_no_list_error(current_token));
      }

      current_list->list.push_back(cell_c(
          cell_type_e::INTEGER, current_token.data, current_token.location));
      return parse(tokens, current_list);
   }

   case token_e::DOUBLE: {
      if (!current_list) {
         return std::make_tuple(cell_c(), get_no_list_error(current_token));
      }

      current_list->list.push_back(cell_c(
          cell_type_e::DOUBLE, current_token.data, current_token.location));
      return parse(tokens, current_list);
   }
   }

   auto error = std::make_shared<error::error_c>(
       current_token.location, current_token.data,
       "internal error > unhandled token type");
   return std::make_tuple(cell_c(), error);
}

product_s parse_line(const char *source_descrption, std::size_t line_number,
                     std::string line) {

   product_s resulting_product;

   auto [tokens, token_err] = tokenize(line_number, line);

   if (token_err) {
      resulting_product.error_info = std::move(token_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }

   auto [cell, parse_err] = parse(tokens);

   if (parse_err) {
      resulting_product.error_info = std::move(parse_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }

   resulting_product.cell = cell;
   resulting_product.error_info = nullptr;
   resulting_product.result = result_e::OKAY;
   return resulting_product;
}

} // namespace parser
} // namespace sauros
