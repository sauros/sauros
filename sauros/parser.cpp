#include "parser.hpp"

#include <vector>
#include <regex>
#include <tuple>
#include <iostream>

namespace sauros {
namespace parser {

enum class token_e {
   L_BRACKET,
   R_BRACKET,
   SYMBOL,
   INTEGER,
   DOUBLE,
   STRING
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
std::tuple<std::vector<token_s>, std::unique_ptr<error::error_c>> tokenize(size_t line_number, const std::string line) {

   std::vector<token_s> tokens;
   for (size_t idx = 0; idx < line.size(); idx++) {
      auto current = line[idx];

      if (std::isspace(current)) {
         continue;
      }

      std::string current_data;
      switch(current) {
         case '[':
         {
            tokens.push_back({
               token_e::L_BRACKET,
               "[",
               {line_number, idx}
            });
            continue;
         }

         case ']':
         {
            tokens.push_back({
               token_e::R_BRACKET,
               "]",
               {line_number, idx}
            });
            continue;
         }

         case '"':{
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

               auto error = std::make_unique<error::error_c>(
                     location_s(line_number, idx), 
                     line.c_str(),
                     "Unterminated string"
               );

               return std::make_tuple(tokens, std::move(error));
            }

            tokens.push_back({
               token_e::STRING,
               value,
               {line_number, start}
            });
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
               tokens.push_back({
                  token_e::DOUBLE,
                  current_data,
                  {line_number, start}
               });
               continue;
            } else {
               tokens.push_back({
                  token_e::INTEGER,
                  current_data,
                  {line_number, start}
               });
               continue;
            }
         } else {
            // Some malformed oddity like: 4444.2323.232.4.34.
            auto error = std::make_unique<error::error_c>(
                  location_s(line_number, start), 
                  line.c_str(),
                  "Malformed representation of suspected numerical"
            );

            return std::make_tuple(tokens, std::move(error));
         }
      }

      // Anything else should be considered a symbol
      //
      std::string value;
      decltype(idx) start = idx;
      while (idx < line.size() && !std::isspace(line[idx]) && line[idx] != '[' && line[idx] != ']') {
         value += line[idx];
         idx++;
      }

      tokens.push_back({
         token_e::SYMBOL,
         value,
         {line_number, start}
      });

      idx--;
   }
   return std::make_tuple(tokens, nullptr);
}

namespace {
   static uint64_t open_counter {0};
}

std::tuple<std::unique_ptr<list_c>, std::unique_ptr<error::error_c>> parse(std::vector<token_s>& tokens, list_c* current_list = nullptr) {

   if (tokens.empty()) {
      return std::make_tuple(nullptr, nullptr);
   }

   auto current_token = tokens[0];
   tokens = std::vector<token_s>(tokens.begin()+1, tokens.end());

   switch(current_token.token) {

      case token_e::L_BRACKET:
      {
         open_counter++;

         // Populate the list
         auto [list, err] = parse(tokens, new list_c());

         if (err) {
            return std::make_tuple(nullptr, std::move(err));
         }

         // If we had  a list, we add our new list to it
         if (current_list) {
            current_list->cells.push_back(std::move(list));

            return parse(tokens, current_list);

         // otherwise we return the new list
         } else {
            return std::make_tuple(std::move(list), nullptr);
         }
      }

      case token_e::R_BRACKET:
      {
         // This means we are done building whatever current_list is
         if (!current_list) {
            auto error = std::make_unique<error::error_c>(
                  current_token.location,
                  current_token.data,
                  "unopened closing bracket `]` located");
            return std::make_tuple(nullptr, std::move(error));
         }

         open_counter--;

         // return the current list as the thing
         return std::make_tuple(std::unique_ptr<list_c>(current_list), nullptr);
      }

      case token_e::SYMBOL:
      {
         if (!current_list) {
            auto error = std::make_unique<error::error_c>(
                  current_token.location,
                  current_token.data,
                  "attempting to create string object prior to list creation");
            return std::make_tuple(nullptr, std::move(error));
         }

         std::unique_ptr<cell_c> new_symbol(new symbol_c(current_token.data));
         current_list->cells.push_back(std::move(new_symbol));

         return parse(tokens, current_list);
      }

      case token_e::STRING:
      {
         if (!current_list) {
            auto error = std::make_unique<error::error_c>(
                  current_token.location,
                  current_token.data,
                  "attempting to create string object prior to list creation");
            return std::make_tuple(nullptr, std::move(error));
         }

         std::unique_ptr<cell_c> new_string(new string_c(current_token.data));
         current_list->cells.push_back(std::move(new_string));

         return parse(tokens, current_list);
      }

      case token_e::INTEGER:
      {
         if (!current_list) {
            auto error = std::make_unique<error::error_c>(
                  current_token.location,
                  current_token.data,
                  "attempting to create string object prior to list creation");
            return std::make_tuple(nullptr, std::move(error));
         }
         std::unique_ptr<cell_c> new_integer(new integer_c(std::stol(current_token.data)));
         current_list->cells.push_back(std::move(new_integer));

         return parse(tokens, current_list);
      }

      case token_e::DOUBLE:
      {
         if (!current_list) {
            auto error = std::make_unique<error::error_c>(
                  current_token.location,
                  current_token.data,
                  "attempting to create string object prior to list creation");
            return std::make_tuple(nullptr, std::move(error));
         }
         std::unique_ptr<cell_c> new_integer(new integer_c(std::stod(current_token.data)));
         current_list->cells.push_back(std::move(new_integer));

         return parse(tokens, current_list);
      }
   }

   auto error = std::make_unique<error::error_c>(
         current_token.location,
         current_token.data,
         "internal error > unhandled token type");
   return std::make_tuple(nullptr, std::move(error));
}


product_s parse_line(const char* source_descrption, std::size_t line_number, std::string line) {

   product_s resulting_product;

   auto [tokens, token_err] = tokenize(line_number, line);

   if (token_err) {
      resulting_product.error_info = std::move(token_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }
   
   auto [cells, parse_err] = parse(tokens);

   if (parse_err) {
      resulting_product.error_info = std::move(parse_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }

   resulting_product.list = std::move(cells);
   resulting_product.error_info = nullptr;
   resulting_product.result = result_e::OKAY;
   return resulting_product;
}

} // namespace parser
} // namespace sauros