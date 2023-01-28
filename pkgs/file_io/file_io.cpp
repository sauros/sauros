#include "file_io.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sauros/capi/capi.hpp>
#include <string>

using namespace sauros;

#define SIZE_CHECK(item__, size__)                                             \
   if (cells.size() != size__) {                                               \
      throw processor_c::runtime_exception_c(                                  \
          sauros::format("`%` expects % parameters", item__, size__ - 1),      \
          cells[0]);                                                           \
   }

#define SELF_CHECK                                                             \
   if (!fc) {                                                                  \
      throw processor_c::runtime_exception_c(                                  \
          "file_io > Target file cell no longer exists", cells[0]);            \
   }

#define EXPECT(condition__, msg__)                                             \
   if (!condition__) {                                                         \
      throw processor_c::runtime_exception_c(msg__, cells[0]);                 \
   }

#define EXPECT_CB(condition__, msg__, cb__)                                    \
   if (!condition__) {                                                         \
      cb__();                                                                  \
      throw processor_c::runtime_exception_c(msg__, cells[0]);                 \
   }

class file_cell_c {
 public:
   std::string name;
   std::fstream *stream{nullptr};
};

// Called on the destruction of a void_cell
void handle_deletion_callback(void *ptr) {
   if (!ptr) {
      return;
   }

   auto file_cell = static_cast<file_cell_c *>(ptr);

   if (file_cell->stream) {
      if (file_cell->stream->is_open()) {
         file_cell->stream->close();
      }

      delete file_cell->stream;
      file_cell->stream = nullptr;
   }

   delete file_cell;

   file_cell = nullptr;
}

extern cell_ptr _pkg_file_io_get_handle_(cells_t &cells,
                                         std::shared_ptr<environment_c> env) {

   // Here we leverage the void_cell to keep a pointer to
   // a file_cell_c so we can manage the memory it takes up
   // without relying on a local copy and so we can determine
   // when its time to clean up the memory

   // Once that is created, a box is created with functions that allow
   // the user to interact with the file_cell_c data.

   // We stuff the void_cell in the box_cell list to ensure it stays
   // alive as long as the box interface to it is alive

   auto void_cell = std::make_shared<void_cell_c>(cells[1]->location);

   auto fc = new file_cell_c();
   void_cell->ptr = static_cast<void *>(fc);
   void_cell->deletion_cb = handle_deletion_callback;

   auto box = std::make_shared<cell_c>(cell_type_e::BOX, cells[0]->location);
   box->inner_env = std::make_shared<sauros::environment_c>();
   box->list.push_back(void_cell);

   fc->name = c_api_process_cell(cells[1], env)->data_as_str();

   box->inner_env->set(
       "open", std::make_shared<cell_c>(
                   [fc](cells_t &cells,
                        std::shared_ptr<environment_c> env) -> cell_ptr {
                      SIZE_CHECK("file_io > open", 1)
                      SELF_CHECK

                      if (fc->stream) {
                         fc->stream->close();
                         delete fc->stream;
                         fc->stream = nullptr;
                      }

                      fc->stream = new std::fstream();
                      fc->stream->open(fc->name, std::ios::in | std::ios::out |
                                                     std::ios::app);

                      EXPECT_CB((fc->stream->is_open()),
                                sauros::format("Unable to open `%`", fc->name),
                                [=]() {
                                   delete fc->stream;
                                   fc->stream = nullptr;
                                })

                      return std::make_shared<sauros::cell_c>(CELL_TRUE);
                   }));

   box->inner_env->set(
       "close", std::make_shared<cell_c>(
                    [fc](cells_t &cells,
                         std::shared_ptr<environment_c> env) -> cell_ptr {
                       SIZE_CHECK("file_io > close", 1)
                       SELF_CHECK

                       if (fc->stream) {
                          fc->stream->close();
                          delete fc->stream;
                          fc->stream = nullptr;
                       }

                       return std::make_shared<sauros::cell_c>(CELL_TRUE);
                    }));

   box->inner_env->set(
       "seek", std::make_shared<cell_c>(
                   [fc](cells_t &cells,
                        std::shared_ptr<environment_c> env) -> cell_ptr {
                      SIZE_CHECK("file_io > seek", 2)
                      SELF_CHECK

                      if (!fc->stream || !fc->stream->is_open()) {
                         return std::make_shared<sauros::cell_c>(CELL_NIL);
                      }

                      auto location = c_api_process_cell(cells[1], env);
                      EXPECT((location->type == cell_type_e::INTEGER),
                             "file_io > seek Expected an integer type")

                      try {
                         fc->stream->seekg(location->data.i);
                      } catch (...) {
                         return std::make_shared<sauros::cell_c>(CELL_FALSE);
                      }
                      return std::make_shared<sauros::cell_c>(CELL_TRUE);
                   }));

   box->inner_env->set(
       "get_pos", std::make_shared<cell_c>(
                      [fc](cells_t &cells,
                           std::shared_ptr<environment_c> env) -> cell_ptr {
                         SIZE_CHECK("file_io > get_pos", 1)
                         SELF_CHECK

                         if (!fc->stream || !fc->stream->is_open()) {
                            return std::make_shared<sauros::cell_c>(CELL_NIL);
                         }
                         return std::make_shared<cell_c>(
                             cell_type_e::INTEGER,
                             (sauros::cell_int_t)fc->stream->tellg(),
                             cells[0]->location);
                      }));

   box->inner_env->set(
       "get_size", std::make_shared<cell_c>(
                       [fc](cells_t &cells,
                            std::shared_ptr<environment_c> env) -> cell_ptr {
                          SIZE_CHECK("file_io > get_size", 1)
                          SELF_CHECK

                          if (!fc->stream || !fc->stream->is_open()) {
                             return std::make_shared<sauros::cell_c>(CELL_NIL);
                          }

                          auto current_pos = fc->stream->tellg();

                          fc->stream->seekg(0, fc->stream->end);

                          auto length = (sauros::cell_int_t)fc->stream->tellg();

                          fc->stream->seekg(current_pos);

                          return std::make_shared<cell_c>(
                              cell_type_e::INTEGER, length, cells[0]->location);
                       }));

   box->inner_env->set(
       "get_n", std::make_shared<cell_c>(
                    [fc](cells_t &cells,
                         std::shared_ptr<environment_c> env) -> cell_ptr {
                       SIZE_CHECK("file_io > get_n", 2)
                       SELF_CHECK

                       if (!fc->stream || !fc->stream->is_open()) {
                          return std::make_shared<sauros::cell_c>(CELL_NIL);
                       }

                       auto n = c_api_process_cell(cells[1], env);
                       EXPECT((n->type == cell_type_e::INTEGER),
                              "file_io > get_n Expected an integer type")

                       auto n_actual = n->data.i;

                       char *buffer = new char[n_actual]();

                       fc->stream->read(buffer, n_actual);

                       std::string result_string(buffer, n_actual);

                       delete buffer;

                       auto result = std::make_shared<cell_c>(
                           cell_type_e::STRING, result_string,
                           cells[0]->location);

                       return result;
                    }));

   box->inner_env->set(
       "get_line", std::make_shared<cell_c>(
                       [fc](cells_t &cells,
                            std::shared_ptr<environment_c> env) -> cell_ptr {
                          SIZE_CHECK("file_io > get_line", 1)
                          SELF_CHECK

                          if (!fc->stream || !fc->stream->is_open()) {
                             return std::make_shared<sauros::cell_c>(CELL_NIL);
                          }

                          std::string data;
                          std::getline(*fc->stream, data);

                          auto result = std::make_shared<cell_c>(
                              cell_type_e::STRING, data, cells[0]->location);

                          return result;
                       }));

   box->inner_env->set(
       "write_string",
       std::make_shared<cell_c>(
           [fc](cells_t &cells,
                std::shared_ptr<environment_c> env) -> cell_ptr {
              SIZE_CHECK("file_io > write_string", 2)
              SELF_CHECK

              auto line = c_api_process_cell(cells[1], env);
              EXPECT((line->type == cell_type_e::STRING),
                     "file_io > write_string Expected a string type")

              if (!fc->stream || !fc->stream->is_open()) {
                 return std::make_shared<sauros::cell_c>(CELL_NIL);
              }

              (*fc->stream) << line->data_as_str();

              return std::make_shared<sauros::cell_c>(CELL_TRUE);
           }));

   return box;
}