#include "parser.hpp"

#include <regex>
#include <tuple>
#include <vector>

#include "builtin_encodings.hpp"

namespace sauros {
namespace parser {

static std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

static bool is_digit(const char c) {
   return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

//    Retrieve a list of tokens based on the given string
//
std::vector<token_s> tokenize(size_t line_number, const std::string line,
                              bracket_track_s &bts,
                              std::shared_ptr<std::string> origin) {

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
         bts.location = {line_number, idx};
         bts.tracker++;
         continue;
      }

      case ']': {
         tokens.push_back({token_e::R_BRACKET, "]", {line_number, idx}});
         if (bts.tracker > 0) {
            bts.tracker--;
         } else {
            throw parser_exception_c("Unmatched closing bracket", origin,
                                     {line_number, idx});
         }
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
            throw parser_exception_c("Unterminated string", origin,
                                     {line_number, idx});
            return tokens;
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
      if (is_digit(line[idx]) ||
          (line[idx] == '-' && is_digit(line[idx + 1]))) {

         decltype(idx) start = idx;
         while (is_digit(line[idx]) || line[idx] == '.' || line[idx] == '-') {
            current_data += line[idx];
            idx++;
         }
         idx--;

         if (std::regex_match(current_data, is_number)) {
            if (current_data.find('.') != std::string::npos) {
               tokens.push_back(
                   {token_e::REAL, current_data, {line_number, start}});
               continue;
            } else {
               tokens.push_back(
                   {token_e::INTEGER, current_data, {line_number, start}});
               continue;
            }
         } else {
            throw parser_exception_c(
                "Malformed representaton of suspected numerical", origin,
                {line_number, start});
            return tokens;
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
   return tokens;
}

namespace {
void throw_no_list_error(token_s current_token,
                         std::shared_ptr<std::string> origin) {
   throw parser_exception_c(
       "Attempting to create object prior to list creation", origin,
       current_token.location);
}
} // namespace

cell_ptr parse(std::vector<token_s> &tokens,
               std::shared_ptr<std::string> origin,
               cell_ptr current_list = nullptr) {

   if (tokens.empty()) {
      return {};
   }

   auto current_token = tokens[0];
   tokens = std::vector<token_s>(tokens.begin() + 1, tokens.end());

   switch (current_token.token) {

   case token_e::L_BRACKET: {
      cell_ptr new_list = std::make_shared<cell_c>(cell_type_e::LIST);
      new_list->origin = origin;

      // Populate the list
      parse(tokens, origin, new_list);

      // If we had  a list, we add our new list to it
      if (current_list) {
         current_list->list.push_back(new_list);

         return parse(tokens, origin, current_list);

         // otherwise we return the new list
      } else {
         return new_list;
      }
   }

   case token_e::R_BRACKET: {
      // This means we are done building whatever current_list is
      if (!current_list) {
         throw parser_exception_c("Unopened closing bracket detected", origin,
                                  current_token.location);
         return {};
      }

      // The cell list being created has been populated recursively so
      // we don't need to return anything here.
      return {};
   }

   case token_e::SYMBOL: {
      if (!current_list) {
         throw_no_list_error(current_token, origin);
         return {};
      }

      // Check the encoding map for builtins to see if we need to
      if (BUILTIN_STRING_TO_ENCODING.find(current_token.data) !=
          BUILTIN_STRING_TO_ENCODING.end()) {
         cell_ptr builtin_translation_cell = std::make_shared<cell_c>(
             cell_type_e::ENCODED_SYMBOL, current_token.data,
             new location_s(current_token.location), origin);
         builtin_translation_cell->builtin_encoding =
             BUILTIN_STRING_TO_ENCODING[current_token.data];
         current_list->list.push_back(builtin_translation_cell);
      } else {
         current_list->list.push_back(std::make_shared<cell_c>(
             cell_type_e::SYMBOL, current_token.data,
             new location_s(current_token.location), origin));
      }
      return parse(tokens, origin, current_list);
   }

   case token_e::STRING: {
      if (!current_list) {
         throw_no_list_error(current_token, origin);
         return {};
      }

      current_list->list.push_back(std::make_shared<cell_c>(
          cell_type_e::STRING, current_token.data,
          new location_s(current_token.location), origin));
      return parse(tokens, origin, current_list);
   }

   case token_e::INTEGER: {
      if (!current_list) {
         throw_no_list_error(current_token, origin);
         return {};
      }

      current_list->list.push_back(std::make_shared<cell_c>(
          cell_type_e::INTEGER, current_token.data,
          new location_s(current_token.location), origin));
      return parse(tokens, origin, current_list);
   }

   case token_e::REAL: {
      if (!current_list) {
         throw_no_list_error(current_token, origin);
         return {};
      }

      current_list->list.push_back(std::make_shared<cell_c>(
          cell_type_e::REAL, current_token.data,
          new location_s(current_token.location), origin));
      return parse(tokens, origin, current_list);
   }
   }

   throw parser_exception_c("internal error > unhandled token type", origin,
                            current_token.location);
   return {};
}

cell_ptr parse_line(const char *source_descrption, std::size_t line_number,
                    std::string line) {
   bracket_track_s bts;
   auto tokens = tokenize(line_number, line, bts, nullptr);
   return parse(tokens, nullptr);
}

void segment_parser_c::set_origin(const std::string &origin) {
   _origin = std::make_shared<std::string>(origin);
}

std::optional<cell_ptr>
segment_parser_c::submit(segment_parser_c::segment_s segment) {

   std::size_t comment_loc = segment.line.find_first_of(";");
   if (comment_loc != std::string::npos) {
      segment.line = segment.line.substr(0, comment_loc);
   }

   if (segment.line.empty()) {
      return {};
   }

   auto tokens = tokenize(segment.line_number, segment.line, _bts, _origin);
   _tokens.insert(_tokens.end(), tokens.begin(), tokens.end());

   if (_bts.tracker == 0 && !_tokens.empty()) {
      return {parse(_tokens, _origin)};
   }
   return {};
}

void segment_parser_c::indicate_complete() {
   if (_bts.tracker != 0) {
      throw parser_exception_c("Unmatched opening bracket", _origin,
                               _bts.location);
   }
}

} // namespace parser
} // namespace sauros
