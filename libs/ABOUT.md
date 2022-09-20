
## About

The `install.py` script will update all existing libraries in the `SAUROS_HOME` directory, 
so an environment variable representing a writeable directory must be present.


## Creating a library

Every library must have a unique name, with a CMakeLists.txt in their top level

Each library must build a shared library file called `sauros. < library name > .lib`

Within each directory representing a library a `manifest.sau` must at least contain the following updated to be specific to said library:

```
; Prefix of the library that will be used to 
; artificially scope functions in the library 
;     (https://github.com/bosley/sauros/wiki/Tutorial#naming-conventions)
[var prefix "std"]

; Name of the library file that needs to be 
; linked to
[var target_name "sauros.std.lib"]

;  List of functions that should be loadable
;  from the shared object that is the external
;  library
[var function_list [list 
   "print"
]]
```
