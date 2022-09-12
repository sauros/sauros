#include "parser.hpp"
#include "ast.hpp"

#include <vector>
#include <regex>
#include <tuple>
#include <iostream>

namespace sauros {
namespace parser {


enum class token_e {
   L_PAREN,
   R_PAREN,
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



void display_tree(const std::string &prefix, ast::node_c *n, bool is_left) {
   if (!n) {
      return;
   }
  
   std::cout << prefix;
   std::cout << (is_left ? "├──" : "└──");
   std::cout << " " << n->data << std::endl;
   display_tree(prefix + (is_left ? "│   " : "    "), n->left.get() ,  true);
   display_tree(prefix + (is_left ? "│   " : "    "), n->right.get(), false);
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
         case '(':
         {
            tokens.push_back({
               token_e::L_PAREN,
               "(",
               {line_number, idx}
            });
            continue;
         }

         case ')':
         {
            tokens.push_back({
               token_e::R_PAREN,
               ")",
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
               } else if (!std::isspace(line[idx]) && line[idx] != '(' &&
                        line[idx] != ')') {
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
      while (idx < line.size() && !std::isspace(line[idx]) && line[idx] != '(' && line[idx] != ')') {
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

// 
//
std::tuple<std::unique_ptr<ast::node_c>, std::unique_ptr<error::error_c>> parse(std::vector<token_s> tokens, bool first = false) {

   if (tokens.empty()) {
      return std::make_tuple(nullptr, nullptr);
   }

   /*
      (define x (+ 3 2))
   
   */

   auto& current_token = tokens[0];

   switch(current_token.token) {

      // Skip this and go to the next token
      case token_e::L_PAREN:
      {
         return parse(std::vector<token_s>(tokens.begin()+1, tokens.end()), true);
      }

      case token_e::R_PAREN:
      {
         return parse(std::vector<token_s>(tokens.begin()+1, tokens.end()));
      }

      case token_e::SYMBOL:
      {
         // If its immediatly following a ( then it is a root
         if (first) {

            auto new_node = std::make_unique<ast::node_c>(current_token.data, ast::node_type_e::SYMBOL);

            auto [left_node, left_e] = parse(std::vector<token_s>(tokens.begin()+1, tokens.end()));
            if (left_e) {
               return std::make_tuple(nullptr, std::move(left_e));
            }

            auto [right_node, right_e] = parse(std::vector<token_s>(tokens.begin()+2, tokens.end()));
            if (right_e) {
               return std::make_tuple(nullptr, std::move(right_e));
            }

            if (!right_node) {
               std::cout << "no right ?? " << std::endl;
            }

            new_node->left = std::move(left_node);
            new_node->right = std::move(right_node);
            
            return std::make_tuple(std::move(new_node), nullptr);


         // If its not immediatly following a ( then it is a stand alone symbol
         //
         } else {
            auto new_node = std::make_unique<ast::node_c>(current_token.data, ast::node_type_e::SYMBOL);
            return std::make_tuple(std::move(new_node), nullptr);
         }
      }

      case token_e::INTEGER:
      {
         auto new_node = std::make_unique<ast::node_c>(current_token.data, ast::node_type_e::INTEGER);
         return std::make_tuple(std::move(new_node), nullptr);
      }
      case token_e::STRING:
      {
         auto new_node = std::make_unique<ast::node_c>(current_token.data, ast::node_type_e::STRING);
         return std::make_tuple(std::move(new_node), nullptr);
      }
      case token_e::DOUBLE:
      {
         auto new_node = std::make_unique<ast::node_c>(current_token.data, ast::node_type_e::DOUBLE);
         return std::make_tuple(std::move(new_node), nullptr);
      }
   }

   auto error = std::make_unique<error::error_c>(
         current_token.location,
         current_token.data,
         "internal error > unable to match item in ast construction");

   return std::make_tuple(nullptr, std::move(error));
}

product_s parse_line(const char* source_descrption, std::size_t line_number, std::string line, bool show_tree) {

   product_s resulting_product;

   auto [tokens, token_err] = tokenize(line_number, line);

   if (token_err) {
      resulting_product.error_info = std::move(token_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }


   auto [tree, parse_err] = parse(tokens);

   if (parse_err) {
      resulting_product.error_info = std::move(parse_err);
      resulting_product.result = result_e::ERROR;
      return resulting_product;
   }

   if (show_tree && tree) {
      display_tree("", tree.get(), true);
   }

   resulting_product.tree = std::move(tree);
   resulting_product.result = result_e::OKAY;
   return resulting_product;
}

} // namespace parser
} // namespace sauros
