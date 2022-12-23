# Packages

These packages are `std` and an example package. The entirity of the std library exists as a single package.

## Layout

Each package must contain `sau` files or `.lib` shared libraries when they are installed. Submodules are allowed in the top-most directory of the package only.


## Example Module - No submodules

File nameed `package.sau` must be in the root of the module directory

```
; Name of the package
[var package_name "my_package"]

; In packages one of, or both `source_files` or `library_file` must exist.
; if `library_file` exists, then so too must `library_functions` to list
; all methods available within the library
;  - If the library exists it will be loaded into the environment
;    prior to `source_files` so the sources can access the functions

; .sau files that should be included in the package
[var source_files [list
   "version.sau"
]]
```

In the event that there are submodules, a directory by that name must exist
and in that directory a `submodule.sau` must exist to detail the submodule.

## Example Module - With submodules

```
; Name of the package
[var package_name "my_package"]

; Indicate that we need to include some submodules
[var submodules [list "sub_module_one" "sub_module_two"]]

; No shared libs at this level (but can be done as above)

; Top level files for the std package
[var source_files [list
   "top_level.sau"
]]

```

### sub_module_one

Within the directory must exist the file `submodule.sau`

```

[var submodule_name "sub_module_one"]

; C++ shared lib to link to
[var library_file "sub_module_one.lib"]
[var library_functions [list
   "example_method"
]]

; Files to include under the current submodule
[var source_files [list
   "some_file.sau"
]]

```