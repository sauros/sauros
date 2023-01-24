#ifndef SAUROS_CELL_HPP
#define SAUROS_CELL_HPP

#include "builtin_encodings.hpp"
#include "types.hpp"
#include <assert.h>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <iostream>

namespace sauros {

//! \brief Types of cells
enum class cell_type_e {
   REAL = 0,
   INTEGER,
   STRING,
   SYMBOL,
   LIST,
   LAMBDA,
   BOX,
   BOX_SYMBOL,
   ENCODED_SYMBOL,
   VARIANT
};

//! \brief Retrieve the type as a string
static const char *cell_type_to_string(const cell_type_e type) {
   switch (type) {
   case cell_type_e::SYMBOL:
      return "symbol";
   case cell_type_e::BOX_SYMBOL:
      return "box_symbol";
   case cell_type_e::LIST:
      return "list";
   case cell_type_e::LAMBDA:
      return "lambda";
   case cell_type_e::STRING:
      return "string";
   case cell_type_e::INTEGER:
      return "integer";
   case cell_type_e::REAL:
      return "real";
   case cell_type_e::BOX:
      return "box";
   case cell_type_e::VARIANT:
      return "variant";
   case cell_type_e::ENCODED_SYMBOL:
      return "encoded_symbol";
   }
   return "unknown";
}

//! \brief Forward of environment for proc_f
class environment_c;

//! \brief Forward for processor for cells with
//!        embedded processors
class processor_c;

//! \brief An cell representation
class cell_c {
 public:
   //! \brief Function pointer definition for a cell
   //!        used to execute code
   using proc_f =
       std::function<cell_ptr(cells_t &, std::shared_ptr<environment_c> env)>;

   //! \brief Create an empty cell
   //! \param type The type to set (Defaults to SYMBOL)
   cell_c(cell_type_e type = cell_type_e::SYMBOL) : type(type) {}

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   cell_c(cell_type_e type, const cell_string_t data) : type(type) {
      assert(type != cell_type_e::INTEGER);
      assert(type != cell_type_e::REAL);
      this->data.s = new std::string(data);
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   cell_c(cell_type_e type, cell_real_t data) : type(type) {
      assert(type == cell_type_e::REAL);
      this->data.d = data;
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   cell_c(cell_type_e type, cell_int_t data) : type(type) {
      assert(type == cell_type_e::INTEGER);
      this->data.i = data;
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location_in The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_string_t data, location_s *location_in)
       : type(type) {
      assert(type != cell_type_e::INTEGER);
      assert(type != cell_type_e::REAL);
      this->data.s = new std::string(data);
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location_in The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_real_t data, location_s *location_in)
       : type(type) {
      assert(type == cell_type_e::REAL);
      this->data.d = data;
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location_in The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_int_t data, location_s *location_in)
       : type(type) {
      assert(type == cell_type_e::INTEGER);
      this->data.i = data;
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param location_in The location in source that the cell originated from
   cell_c(cell_type_e type, location_s *location_in) : type(type) {
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_string_t data, location_s *location_in,
          std::shared_ptr<std::string> origin)
       : type(type), origin(origin) {
      assert(type != cell_type_e::INTEGER);
      assert(type != cell_type_e::REAL);
      this->data.s = new std::string(data);
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_real_t data, location_s *location_in,
          std::shared_ptr<std::string> origin)
       : type(type), origin(origin) {
      assert(type == cell_type_e::REAL);
      this->data.d = data;
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location The location in source that the cell originated from
   cell_c(cell_type_e type, const cell_int_t data, location_s *location_in,
          std::shared_ptr<std::string> origin)
       : type(type), origin(origin) {
      assert(type == cell_type_e::INTEGER);
      this->data.i = data;
      if (location_in) {
         location = new location_s(*location_in);
      }
   }

   //! \brief Create a process cell
   //! \param proc The process function to set
   //! \note Process cells are declared as SYMBOL to reduce number of
   //!       given celltypes. A process cell can be quickly identified
   //!       by the existence of a valid proc_f being set
   cell_c(proc_f proc) : type(cell_type_e::SYMBOL), proc(proc) {
      data.s = nullptr;
   }

   //! \brief Create a list cell
   //! \param list The list to set in the cell
   cell_c(cells_t list) : type(cell_type_e::LIST), list(list) {
      data.s = nullptr;
   }

   //! \brief Clone the cell
   //! \returns A new copy of the cell in a shared_ptr
   cell_ptr clone() const { return std::shared_ptr<cell_c>(new cell_c(*this)); }

   //! \brief Explicit copy constructor for cell
   cell_c(const cell_c &other) {
      switch (other.type) {
      case cell_type_e::REAL:
         data.d = other.data.d;
         break;
      case cell_type_e::INTEGER:
         data.i = other.data.i;
         break;
      default:
         if (other.data.s) {
            data.s = new std::string(*other.data.s);
         }
         break;
      }
      type = other.type;
      if (other.location) {
         location = new location_s(*other.location);
      }
      proc = other.proc;
      list = other.list;
      inner_env = other.inner_env;
      builtin_encoding = other.builtin_encoding;
      origin = other.origin;
   }

   //! \brief Clean up the cell
   virtual ~cell_c() {
      if (location) {
         delete location;
         location = nullptr;
      }
      if ((type != cell_type_e::INTEGER && type != cell_type_e::REAL) &&
          data.s) {
         delete data.s;
         data.s = nullptr;
      }
   }

   std::string data_as_str() const {
      switch (type) {
      case cell_type_e::INTEGER:
         return std::to_string(data.i);
      case cell_type_e::REAL:
         return std::to_string(data.d);
      case cell_type_e::STRING:
         return *data.s;
      default: {
         if (data.s) {
            return *data.s;
         }
      }
      }
      return "";
   }

   union data_u {
      constexpr data_u() : s{nullptr} {}
      ~data_u() {}
      cell_int_t i;
      cell_real_t d;
      cell_string_t *s;
   };

   // Data
   data_u data;
   cell_type_e type{cell_type_e::SYMBOL};
   location_s *location{nullptr};
   proc_f proc{nullptr};
   cells_t list;
   std::shared_ptr<environment_c> inner_env{nullptr};
   uint8_t builtin_encoding{BUILTIN_DEFAULT_VAL};
   std::shared_ptr<std::string> origin{nullptr};
};

static const cell_c CELL_TRUE = cell_c(
    cell_type_e::INTEGER,
    (cell_int_t)1); //! A cell that represents TRUE - cast to avoid ambiguity
static const cell_c CELL_FALSE = cell_c(
    cell_type_e::INTEGER,
    (cell_int_t)0); //! A cell that represents FALSE - cast to avoid ambiguity
static const cell_c CELL_NIL =
    cell_c(cell_type_e::STRING, "#nil"); //! A cell that represents NIL

enum class cell_variant_type_e { ASYNC, THREAD, CHAN, REF, VOID };

//! \brief A variant of cell_c
//!        The variant type is meant to be able to extend cells
//!        for very specific uses that enable us to not store more
//!        and more data in the cell_c. This way we can abstract
//!        specific things away and not add more and more cell types
//!        and data as that slows down the processing.
class variant_cell_c : public cell_c {
 public:
   variant_cell_c(cell_variant_type_e type, location_s *location)
       : cell_c(cell_type_e::VARIANT, location), variant_type(type) {}
   cell_variant_type_e variant_type;
};

using variant_cell_ptr = std::shared_ptr<variant_cell_c>;

//! \brief A cell used to encapuslate the operations of an
//!        asynchronus operation.This allows us to embed the
//!        methods and data used to interact with async
//!        processes without creating new storage containers
//!        within the processor or environment
class async_cell_c : public variant_cell_c {
 public:
   async_cell_c(location_s *location);
   std::shared_ptr<processor_c> processor;
   std::future<cell_ptr> future;
   cell_ptr get_fn;
   cell_ptr wait_fn;
};

//! \brief A cell used to encapuslate the operations of an
//!        thread.This allows us to embed the
//!        methods and data used to interact with thread
//!        without creating new storage containers
//!        within the processor or environment
class thread_cell_c : public variant_cell_c {
 public:
   thread_cell_c(location_s *location);
   std::shared_ptr<processor_c> processor;
   std::thread thread;
   cell_ptr is_joinable;
   cell_ptr join;
   cell_ptr detach;
   cell_ptr get_id;
};

//! \brief A cell used to propagate data in a safe way
//!        between async functions
class chan_cell_c : public variant_cell_c {
 public:
   chan_cell_c(location_s *location);
   std::mutex channel_mutex;
   std::queue<cell_ptr> channel_queue;
   std::shared_ptr<processor_c> processor;
   cell_ptr put_fn;
   cell_ptr has_data_fn;
   cell_ptr get_fn;
   cell_ptr drain_fn;
};

//! \brief A cell that can be used as a safe reference
class ref_cell_c : public variant_cell_c {
 public:
   ref_cell_c(location_s *location);
   std::mutex ref_mut;
   cell_ptr ref_value;
   cell_ptr put_fn;
   cell_ptr get_fn;
};

//! \brief A cell that can be leveraged to store data
//!        of any kind (for external use)
class void_cell_c : public variant_cell_c {
 public:
   void_cell_c(location_s *location);
   ~void_cell_c();

   // The entity that utilizes this pointer
   // is responsible for maintaining the
   // data it points
   void *ptr{nullptr};

   // Callback that, if set, will be executed upon
   // the destruction of the cell
   std::function<void(void *)> deletion_cb{nullptr};
};

} // namespace sauros

#endif