#include "processor.hpp"
#include "sauros/driver.hpp"
#include "sauros/format.hpp"
#include "sauros/profiler.hpp"

#include <filesystem>

namespace sauros {

bool processor_c::load_file(std::string file, cell_ptr cell,
                            std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::load_file");
#endif

   file_executor_c loader(env);

   // std::cout << "cell value : " << cell_value << std::endl;
   //  Try direct
   if (!loader.run(file)) {
      // success
      return true;
   }

   if (cell->origin) {
      // Try with cell origin in front of it
      std::filesystem::path full_path = (*cell->origin);
      full_path.remove_filename();
      full_path /= file;

      // std::cout << "full path value : " << full_path << std::endl;
      if (!loader.run(full_path)) {
         // success
         return true;
      }
   }

   auto sauros_dir = _system.get_sauros_directory();

   // Try saurs home dir
   if (sauros_dir.has_value()) {
      std::filesystem::path p = (*sauros_dir);
      p /= file;
      // std::cout << "attempting : " << p << std::endl;
      return !loader.run(p);
   }
   // std::cout << "fail\n";
   return false;
}

} // namespace sauros
