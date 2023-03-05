#include "fmt.hpp"
#include <sauros/capi/capi.hpp>
#include <string>

namespace {

inline void check_buffer(std::string &buffer, char c) {
  switch (c) {
  case 'n':
    buffer += '\n';
    break;
  case 't':
    buffer += '\t';
    break;
  case 'r':
    buffer += '\r';
    break;
  case 'a':
    buffer += '\a';
    break;
  case 'b':
    buffer += '\b';
    break;
  case 'v':
    buffer += '\v';
    break;
  case '?':
    buffer += '\?';
    break;
  case '"':
    buffer += '\"';
    break;
  case '\'':
    buffer += '\'';
    break;
  case '%':
    buffer += '%';
    break;
  case '\\':
    buffer += '\\';
    break;
  default:
    break;
  }
}

} // namespace

sauros::cell_ptr
_pkg_fmt_format_encode_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {

  auto raw_target = c_api_process_cell(cells[1], env);
  auto raw_source = c_api_process_cell(cells[2], env);

  if (raw_target->type != sauros::cell_type_e::STRING) {
    throw sauros::exceptions::runtime_c(
        "format string expects source cell to be a string", raw_target);
  }

  auto target = raw_target->data_as_str();

  sauros::cells_t source_cells;
  {
    auto source = raw_source;
    if (source->type == sauros::cell_type_e::LIST) {
      source_cells.reserve(source->list.size());
      for (auto &c : source->list) {
        source_cells.push_back(c_api_cell_to_string(c, env));
      }
    } else {
      source_cells.push_back(source);
    }
  }

  std::string buffer;

  size_t emplaced = 0;
  for (auto i = 0; i < target.size(); i++) {

    auto c = target[i];

    if (c == '\\') {
      if (i == target.size() - 1) {
        throw sauros::exceptions::runtime_c(
            "formated string ends with escape character", cells[1]);
      }
      check_buffer(buffer, target[i + 1]);
      i++;
      continue;
    }

    if (c == '%') {
      if (emplaced >= source_cells.size()) {
        throw sauros::exceptions::runtime_c(
            "not enough source cells given to format string", cells[1]);
      }

      auto result = c_api_cell_to_string(source_cells[emplaced++], env);

      buffer += result->data_as_str();
      continue;
    }

    buffer += c;
  }

  return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, buffer);
}

sauros::cell_ptr
_pkg_fmt_format_string_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {

  auto raw_target = c_api_process_cell(cells[1], env);

  if (raw_target->type != sauros::cell_type_e::STRING) {
    throw sauros::exceptions::runtime_c(
        "format string expects source cell to be a string", raw_target);
  }

  auto target = raw_target->data_as_str();

  std::string buffer;
  for (auto i = 0; i < target.size(); i++) {

    auto c = target[i];

    if (c == '\\') {
      if (i == target.size() - 1) {
        throw sauros::exceptions::runtime_c(
            "formated string ends with escape character", cells[1]);
      }
      check_buffer(buffer, target[i + 1]);
      i++;
      continue;
    }
    buffer += c;
  }

  return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, buffer);
}

sauros::cell_ptr _pkg_fmt_join_(sauros::cells_t &cells,
                                std::shared_ptr<sauros::environment_c> env) {
  auto source = c_api_process_cell(cells[1], env);
  if (source->type != sauros::cell_type_e::LIST) {
    throw sauros::exceptions::runtime_c("join expects source cell to be a list",
                                        cells[1]);
  }

  std::string result;
  for (auto &e : source->list) {
    auto string_cell = c_api_cell_to_string(e, env);
    result += string_cell->data_as_str();
  }

  return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, result);
}

sauros::cell_ptr _pkg_fmt_expand_(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env) {

  auto source = c_api_process_cell(cells[1], env);

  sauros::cell_ptr result =
      std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

  for (auto &c : source->data_as_str()) {
    result->list.push_back(std::make_shared<sauros::cell_c>(
        sauros::cell_type_e::STRING, std::string(1, c)));
  }
  return result;
}

sauros::cell_ptr _pkg_fmt_split_(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env) {

  auto source = c_api_process_cell(cells[1], env);
  if (source->type != sauros::cell_type_e::STRING) {
    throw sauros::exceptions::runtime_c(
        "split expects source cell to be a string", cells[1]);
  }

  auto delimiter = c_api_process_cell(cells[2], env);
  if (delimiter->type != sauros::cell_type_e::STRING) {
    throw sauros::exceptions::runtime_c(
        "split expects delimiter cell to be a string", cells[2]);
  }

  sauros::cell_ptr result =
      std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

  std::string buffer;
  for (auto &c : source->data_as_str()) {
    if (std::string(1, c) == delimiter->data_as_str()) {
      result->list.push_back(std::make_shared<sauros::cell_c>(
          sauros::cell_type_e::STRING, buffer));
      buffer.clear();
    } else {
      buffer += c;
    }
  }

  if (!buffer.empty()) {
    result->list.push_back(
        std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, buffer));
    buffer.clear();
  }

  return result;
}