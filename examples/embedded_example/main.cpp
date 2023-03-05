#include <filesystem>
#include <iostream>
#include <sauros/sauros.hpp>

int main(int argc, char **argv) {

  std::shared_ptr<sauros::environment_c> env =
      std::make_shared<sauros::environment_c>();

  sauros::file_executor_c executor(env);

  if (!std::filesystem::is_regular_file("example.saur")) {
    std::cerr << "Given item `example.saur` is not a file" << std::endl;
    std::exit(1);
  }

  executor.run("example.saur");

  // Pull data from the environment

  if (env->exists("name")) {
    auto name_cell = env->get("name");
    std::cout << name_cell->data_as_str() << std::endl;
  }

  if (env->exists("age")) {
    auto age_cell = env->get("age");
    std::cout << age_cell->data_as_str() << std::endl;
  }

  return 0;
}