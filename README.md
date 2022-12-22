<p align="center">
  <img src="https://img.icons8.com/ios-glyphs/90/null/lizard.png"/>
</p>

# Sauros


[![GitHub license](https://badgen.net/github/license/Naereen/Strapdown.js)](https://github.com/bosley/sauros/blob/main/LICENSE)
[![CircleCI](https://dl.circleci.com/status-badge/img/gh/bosley/sauros/tree/main.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/bosley/sauros/tree/main)

Sauros is an interpreted language based on list execution, and under heavy development. 

Check out the [wiki](https://github.com/bosley/sauros/wiki) to see what has been implemented so far!

The goal of Sauros is to have an easy to use lisp-like scripting language with updated terminiology and features. 
There is a distant hope of having sauros also be compiled, but that will held off until a VM and feature lock is established in a base implementation. 

## Building

Building Sauros is simple - there are no external dependencies aside from he library CPPUTest for unit tests (which can be disabled).

### Installing CPPUTest:

*Ubuntu/ Mac/ etc*

`./sauros/scripts/install_cpputest.sh`

### Building Sauros

```
git clone git@github.com:bosley/sauros.git
mkdir build 
cd build
cmake ../src
make -j5
```

## Installing 

Sauros can be installed like any language. 

1) Ensure that `SAUROS_HOME` is defined in your enviornment
2) Follow the build instructions and ensure the `SETUP_SAUROS` option is set to `ON`
3) Run `make` and `make install` 

The sauros binary will be installed along with all of the sauros libraries. 

If you only want the libraries, and not to install the binary, `skip step 2`.

## Embedding Sauros

Embedding sauros into another application can now be done with a `CMakeLists.txt` as follows:

```
find_package(libsauros REQUIRED)
include_directories(${LIBAUROS_INCLUDE_DIRS})
```

and then link the libraries to yourr application with:

```
target_link_libraries(YOUR-PROJECT-NAME-HERE
   PRIVATE
   ${LIBSAUROS_LIBRARIES})
```

Check out the `embedded_example` in the Sauros root directory to see how this is done and how Sauros is used by an external application.

## Using the stdlib

The stdlib is not very expansive, and won't be for some time, but there still exist some functions!

If Sauros is installed as described above with `step 2` included then you should be able to access the standard library using the `import` instruction as so:

```
[import "std/cli"]

[var an_integer [cli::get.int "please enter an int> "]]

[putln "You gave the valid integer: " an_integer]

```


## Helping out

If you want to hop on Sauros development check out open issues or send me a message!

## Submitting Bugs

When submitting a bug please include the output from `sauros --system`. This will provide potentially helpful information about your system and the build of sauros that you were using when the bug popped up. In addition to this please list the following:

```
  Description of the issue:

  Code snippit that reproduces the issue:
```


<a target="_blank" href="https://icons8.com/icon/103610/lizard">Lizard icon by Icons8</a>

