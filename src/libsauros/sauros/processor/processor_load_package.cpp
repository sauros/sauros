#include "processor.hpp"
#include "sauros/driver.hpp"
#include "sauros/package/package.hpp"
#include "sauros/profiler.hpp"
#include <filesystem>
#include <iostream>

#include <RLL/rll_wrapper.hpp>

namespace sauros {

// The RLL object blows up if its in a header, likely because
// of the use of .inl files for platform specifics. Because of
// that we forward declare the type in the header for the processor
// then we operate wih it here.

// Investigating the issue is backlogged.

void processor_c::load_package(cell_ptr cell, location_s *location,
                               env_ptr env) {
#ifdef PROFILER_ENABLED
  profiler_c::get_profiler()->hit("processor_c::load_package");
#endif

  auto target = cell->data_as_str();
  // Check to see if its already loaded
  if (env->package_loaded(target)) {
    return;
  }

  auto pkg = package::load(cell, _system, location, env);

  // Load any required packages
  for (auto &required_package : pkg.requires_list) {
    load_package(std::make_shared<cell_c>(cell_type_e::STRING, required_package,
                                          cell->location),
                 location, env);
  }

  auto package_rll = std::make_shared<rll_wrapper_c>();

  // Make the cell that will encompass all imports
  auto boxed_cell = std::make_shared<cell_c>(cell_type_e::BOX);
  boxed_cell->inner_env = std::make_shared<environment_c>();

  //
  //    Load the library
  //
  try {
    package_rll->load(pkg.library_file.c_str());
  } catch (rll_wrapper_c::library_loading_error_c &e) {
    throw exceptions::runtime_c(
        "error loading `" + target + "`: " + e.what(),
        std::make_shared<cell_c>(cell_type_e::STRING, "", location));
  }

  if (!package_rll->is_loaded()) {
    throw exceptions::runtime_c(
        "failed to load library: `" + pkg.library_file + "`",
        std::make_shared<cell_c>(cell_type_e::STRING, "", location));
  }

  for (auto &f : pkg.library_function_list) {
    if (!package_rll->has_symbol(f)) {
      throw exceptions::runtime_c(
          "error loading `" + target + "`: listed function `" + f +
              "` was not found in `" + pkg.library_file.c_str() + "`",
          std::make_shared<cell_c>(cell_type_e::STRING, "", location));
    }

    void *fn_ptr = package_rll->get_symbol(f);
    cell_c::proc_f fn = reinterpret_cast<cell_ptr (*)(
        cells_t &, std::shared_ptr<environment_c>)>(fn_ptr);

    // Add to env
    boxed_cell->inner_env->set(f, std::make_shared<cell_c>(fn));
  }

  //
  //    Load the source files
  //
  for (auto &f : pkg.source_file_list) {

    // std::cout << "Loading source file : " << f << std::endl;

    sauros::file_executor_c file_executor(boxed_cell->inner_env);
    if (0 != file_executor.run(f)) {
      throw exceptions::runtime_c(
          "unable to open file " + std::string(f),
          std::make_shared<cell_c>(cell_type_e::STRING, "", location));
    }
  }

  //
  //    Add the library to the main enviornment
  //
  env->set(pkg.name, boxed_cell);

  //
  //    Save the rll object to env
  //
  env->save_package(target, package_rll);
}

} // namespace sauros