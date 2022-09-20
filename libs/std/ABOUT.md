Here lies the Sauros standard lib. 

As an example for making libs, this is how this is intended to work,


a folder called `std` should be made under a home `.sauros` directory. This
will store all libs by folder name. Under `std` will be a copy of the local `manifest.sau` which will 
describe the library functions and what file name to look for.

The functions list in the manifest will correlate to all functions that need to be loaded via RLL
from the shared library object. 

```

.sauros
   |
   |\------ installed.sau
   |\------ /std
   |           |\---- manifest.sau
   |           |\---- sauros.std.lib
   |
   |\------ /os
              |\---- manifest.sau
              |\---- sauros.os.lib


```