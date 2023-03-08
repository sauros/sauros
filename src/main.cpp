#include <libsauros/sauros.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "creator.hpp"
#include "dir_loader.hpp"

namespace {
std::shared_ptr<sauros::environment_c> env =
    std::make_shared<sauros::environment_c>();

sauros::repl_c *repl{nullptr};
sauros::file_executor_c *file_executor{nullptr};

} // namespace

void run_file(const std::string &file) {
  if (!std::filesystem::is_regular_file(file)) {
    std::cerr << "Given item `" << file << "` is not a file" << std::endl;
    std::exit(1);
  }

  file_executor = new sauros::file_executor_c(env);
  file_executor->run(file);

  // Indicate that we are about to quit
  file_executor->finish();
}

void run_directory(std::filesystem::path dir) {

  // Change active dir to the app dir
  std::filesystem::current_path(dir);

  // Launch the app
  run_file(app::load_dir(env));
}

void show_help() {

  std::string help = R"(

<filename>               Execute file
--help      -h           Show help
--version   -v           Show version info
--new-cpp-package <name> Create a new package with cpp shared-lib boilerplate
--new-package     <name> Create a new package
--new-app         <name> Create a new sauros application

   )";
  std::cout << help << std::endl;
}

void version_info() {
  std::cout << std::left << std::setw(20) << "version:";
  std::cout << rang::fg::cyan << LIBSAUROS_VERSION << rang::fg::reset
            << std::endl;

  std::cout << std::left << std::setw(20) << "build:";
  std::cout << rang::fg::cyan << get_build_hash() << rang::fg::reset
            << std::endl;
}

int main(int argc, char **argv) {
  std::vector<std::string> args(argv + 1, argv + argc);

  for (size_t i = 0; i < args.size(); i++) {
    if (args[i] == "--help" || args[i] == "-h") {
      show_help();
      return 0;
    }

    if (args[i] == "--version" || args[i] == "-v") {
      version_info();
      return 0;
    }

    if (args[i] == "--new-package") {
      if (i + 1 >= args.size()) {
        std::cerr << "Expected <name> for command `--new-package`" << std::endl;
        return 1;
      }
      i++;
      return app::create_package(args[i]);
    }

    if (args[i] == "--new-cpp-package") {
      if (i + 1 >= args.size()) {
        std::cerr << "Expected <name> for command `--new-cpp-package`"
                  << std::endl;
        return 1;
      }
      i++;
      return app::create_package(args[i], true);
    }

    if (args[i] == "--new-app") {
      if (i + 1 >= args.size()) {
        std::cerr << "Expected <name> for command `--new-app`" << std::endl;
        return 1;
      }
      i++;
      return app::create_app(args[i]);
    }
  }

  env->set("@version",
           std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                            std::string(LIBSAUROS_VERSION)));
  env->set("@build",
           std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                            std::string(get_build_hash())));

  if (args.empty()) {
    auto repl = sauros::repl_c(env);
    repl.start();
    return 0;
  }

  // Create the arguments cell
  std::vector<std::string> program_args(args.begin() + 1, args.end());
  auto args_cell = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
  for (auto &a : program_args) {
    args_cell->list.push_back(
        std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, a));
  }
  env->set("@args", args_cell);
  env->set("@entry_file", std::make_shared<sauros::cell_c>(
                              sauros::cell_type_e::STRING, args[0]));

  // Retrieve any data piped into the program
  auto piped_cell = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
  if (!isatty(STDIN_FILENO)) {
    std::string piped_in;
    while (getline(std::cin, piped_in)) {
      piped_cell->list.push_back(std::make_shared<sauros::cell_c>(
          sauros::cell_type_e::STRING, piped_in));
    }
  }
  env->set("@piped", piped_cell);

  std::filesystem::path target(args[0]);

  if (!std::filesystem::exists(target)) {
    std::cerr << "Given target: " << target << " does not exist" << std::endl;
    return 1;
  }

  if (std::filesystem::is_directory(target)) {
    run_directory(target);
  } else {
    run_file(args[0]);
  }
  return 0;
}