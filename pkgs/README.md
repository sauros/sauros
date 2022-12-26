# Sauros Packages

Packages are an easy way to extend the reach of Sauros


### Layout

Within each package there must exist a `pkg.sau` file that describes the package so it can be loaded by sauros. 
A package may contain a group of `.sau` files and/or a shared library. 

Example:

```
; Pkg name
[var pkg_name "io"]

; Optional C++ shared lib to link
[var library_file "io.lib"]

; If a library_file exists, then so too must a library_functions which 
; details the names of the given functions that the library has to offer
[var library_functions [list
   "_pkg_io_getline_str_"
   "_pkg_io_getline_real_"
   "_pkg_io_getline_int_"
]]

; Optional inclusion of a source_files listing that will import the files 
; into sauros
[var source_files [list
   "prompt.sau"
]]

```

### Library Function Signatures

All functions offered by a library must follow a signature that can be used in a `process cell`, that is, a cell that can be called on. That signature is as follows:

```
sauros::cell_ptr
_my_function_name_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env);
```

For examples on function signatures and how to process cells for a given function, check out the package `io` in `pkgs` as a simple example.


### Loading

When the package is loaded, the library file is the first thing to be brought into the environment that will encapsulate the package. This allows the library to be used by the files brought in via the `source_files` list. 

This feature was created to ensure that unique function names could be abstracted away to sauros specific function names (i.e _pkg_io_getline_real_ -> get_real)

### Access

When a package is imported, it is done so into a `box` cell. That is, a cell that contains an environment that can be accessed by the `.` operator from an outer environment. This is why in `prompt.sau` under the `io` package we see the use of `io.` in the lambda body - lambdas have their own environment as well so `.` is required to access the `io` environment specifically. 

### Example Usage

```
[use "io"]
[use "random"]

[var min [io.get_int "min> "]]
[var max [io.get_int "max> "]]

[putln "Random number: " [random.uniform_int min max]]
```