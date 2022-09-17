#include "sauros/sauros.hpp"
#include "load.hpp"
#include "repl.hpp"

#include <csignal>
#include <filesystem>
#include <iostream>

namespace {
std::shared_ptr<sauros::environment_c> env =
    std::make_shared<sauros::environment_c>();

app::repl_c *repl{nullptr};
app::load_c *load{nullptr};
} // namespace

// Add some environment variables to the system
//    - This serves as a good example of how to externally extend the language
//
void setup_env() {
   env->set("@version",
            sauros::cell_c([=](std::vector<sauros::cell_c> &cells,
                               std::shared_ptr<sauros::environment_c> env)
                               -> std::optional<sauros::cell_c> {
               if (cells.size() != 1) {
                  throw sauros::processor_c::runtime_exception_c(
                      "`__version` expects no arguments, but " +
                          std::to_string(cells.size() - 1) + " were given",
                      cells[0].location);
               }

               return {sauros::cell_c(sauros::cell_type_e::STRING,
                                      std::string(LIBSAUROS_VERSION),
                                      cells[0].location)};
            }));

   env->set("@import",
            sauros::cell_c([=](std::vector<sauros::cell_c> &cells,
                               std::shared_ptr<sauros::environment_c> env)
                               -> std::optional<sauros::cell_c> {
               app::load_c loader;
               for (auto i = cells.begin() + 1; i < cells.end(); i++) {
                  if ((*i).type != sauros::cell_type_e::STRING) {
                     throw sauros::processor_c::runtime_exception_c(
                         "Import objects are expected to be raw strings",
                         (*i).location);
                  }
                  if (0 != loader.run((*i).data, env)) {
                     throw sauros::processor_c::runtime_exception_c(
                         "Unable to load import: " + (*i).data, (*i).location);
                  }
               }
               return {sauros::CELL_TRUE};
            }));
}

void run_file(const std::string &file) {
   if (!std::filesystem::is_regular_file(file)) {
      std::cerr << "Given item `" << file << "` is not a file" << std::endl;
      std::exit(1);
   }

   load = new app::load_c();
   load->run(file, env);
}

void show_help() {

   std::string help = R"(

<filename>           Execute file
--help      -h       Show help
--version   -v       Show version info
   )";
   std::cout << help << std::endl;
}

void handle_signal(int signal) {
   if (repl) {
      repl->stop();
   }

   if (load) {
      delete load;
   }

   std::exit(0);
}

int main(int argc, char **argv) {

   std::vector<std::string> args(argv + 1, argv + argc);

   signal(SIGHUP, handle_signal);  /* Hangup the process */
   signal(SIGINT, handle_signal);  /* Interrupt the process */
   signal(SIGQUIT, handle_signal); /* Quit the process */
   signal(SIGILL, handle_signal);  /* Illegal instruction. */
   signal(SIGTRAP, handle_signal); /* Trace trap. */
   signal(SIGABRT, handle_signal); /* Abort. */

   setup_env();

   for (size_t i = 0; i < args.size(); i++) {
      if (args[i] == "--help" || args[i] == "-h") {
         show_help();
         return 0;
      }

      if (args[i] == "--version" || args[i] == "-v") {
         std::cout << LIBSAUROS_VERSION << std::endl;
         return 0;
      }
   }

   if (args.empty()) {
      repl = new app::repl_c(env);
      repl->start();
      delete repl;
   }

   for (auto arg : args) {
      run_file(arg);
   }

   return 0;
}