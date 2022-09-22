#include "sauros/sauros.hpp"

#include <csignal>
#include <filesystem>
#include <iostream>

#include "hwinfo/hwinfo.h"

namespace {
std::shared_ptr<sauros::environment_c> env =
    std::make_shared<sauros::environment_c>();

sauros::repl_c *repl{nullptr};
sauros::file_executor_c *file_executor{nullptr};

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
                      "`@version` expects no arguments, but " +
                          std::to_string(cells.size() - 1) + " were given",
                      cells[0].location);
               }

               return {sauros::cell_c(sauros::cell_type_e::STRING,
                                      std::string(LIBSAUROS_VERSION),
                                      cells[0].location)};
            }));

   env->set("@build",
            sauros::cell_c([=](std::vector<sauros::cell_c> &cells,
                               std::shared_ptr<sauros::environment_c> env)
                               -> std::optional<sauros::cell_c> {
               return {sauros::cell_c(sauros::cell_type_e::STRING,
                                      std::string(get_build_hash()),
                                      cells[0].location)};
            }));
}

void run_file(const std::string &file) {
   if (!std::filesystem::is_regular_file(file)) {
      std::cerr << "Given item `" << file << "` is not a file" << std::endl;
      std::exit(1);
   }

   file_executor = new sauros::file_executor_c(env);
   file_executor->run(file);
}

void show_help() {

   std::string help = R"(

<filename>           Execute file
--help      -h       Show help
--version   -v       Show version info
--system    -s       Retrieve system information (for bug reports)
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

void system_report() {
   std::cout << rang::fg::green
             << "--------------------------------- SAUROS "
                "----------------------------------"
             << rang::fg::reset << std::endl;
   version_info();

   hwinfo::OS os;
   std::cout << rang::fg::green
             << "----------------------------------- OS "
                "------------------------------------"
             << rang::fg::reset << std::endl;
   std::cout << std::left << std::setw(20) << "operating system:";
   std::cout << os.fullName() << std::endl;
   std::cout << std::left << std::setw(20) << "short name:";
   std::cout << os.name() << std::endl;
   std::cout << std::left << std::setw(20) << "version:";
   std::cout << os.version() << std::endl;
   std::cout << std::left << std::setw(20) << "kernel:";
   std::cout << os.kernel() << std::endl;
   std::cout << std::left << std::setw(20) << "architecture:";
   std::cout << (os.is32bit() ? "32 bit" : "64 bit") << std::endl;
   std::cout << std::left << std::setw(20) << "endianess:";
   std::cout << (os.isLittleEndian() ? "little endian" : "big endian")
             << std::endl;

   hwinfo::CPU cpu;
   std::cout << rang::fg::green
             << "----------------------------------- CPU "
                "-----------------------------------"
             << rang::fg::reset << std::endl;
   std::cout << std::left << std::setw(20) << "vendor:";
   std::cout << cpu.vendor() << std::endl;
   std::cout << std::left << std::setw(20) << "model:";
   std::cout << cpu.modelName() << std::endl;
   std::cout << std::left << std::setw(20) << "physical cores:";
   std::cout << cpu.numPhysicalCores() << std::endl;
   std::cout << std::left << std::setw(20) << "logical cores:";
   std::cout << cpu.numLogicalCores() << std::endl;
   std::cout << std::left << std::setw(20) << "max frequency:";
   std::cout << cpu.maxClockSpeed_kHz() << std::endl;
   std::cout << std::left << std::setw(20) << "regular frequency:";
   std::cout << cpu.regularClockSpeed_kHz() << std::endl;
   std::cout << std::left << std::setw(20) << "current frequency:";
   std::cout << hwinfo::CPU::currentClockSpeed_kHz() << std::endl;
   std::cout << std::left << std::setw(20) << "cache size:";
   std::cout << cpu.cacheSize_Bytes() << std::endl;

   hwinfo::RAM ram;
   std::cout << rang::fg::green
             << "----------------------------------- RAM "
                "-----------------------------------"
             << rang::fg::reset << std::endl;
   std::cout << std::left << std::setw(20) << "vendor:";
   std::cout << ram.vendor() << std::endl;
   std::cout << std::left << std::setw(20) << "model:";
   std::cout << ram.model() << std::endl;
   std::cout << std::left << std::setw(20) << "name:";
   std::cout << ram.name() << std::endl;
   std::cout << std::left << std::setw(20) << "serial-number:";
   std::cout << ram.serialNumber() << std::endl;
   std::cout << std::left << std::setw(20) << "size [MiB]:";
   std::cout << static_cast<double>(ram.totalSize_Bytes()) / 1024.0 / 1024.0
             << std::endl;

   std::cout << rang::fg::green
             << "--------------------------------------------------------------"
                "-------------"
             << rang::fg::reset << std::endl;
}

void handle_signal(int signal) {
   if (repl) {
      repl->stop();
   }

   if (file_executor) {
      delete file_executor;
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
         version_info();
         return 0;
      }

      if (args[i] == "--system" || args[i] == "-s") {
         system_report();
         return 0;
      }
   }

   if (args.empty()) {
      repl = new sauros::repl_c(env);
      repl->start();
      delete repl;
   }

   for (auto arg : args) {
      run_file(arg);
   }

   return 0;
}