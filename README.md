<p align="center">
  <img src="https://img.icons8.com/ios-glyphs/90/null/lizard.png"/>
</p>

# Sauros


[![GitHub license](https://badgen.net/github/license/Naereen/Strapdown.js)](https://github.com/bosley/sauros/blob/main/LICENSE)
[![CircleCI](https://dl.circleci.com/status-badge/img/gh/sauros/sauros/tree/main.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/sauros/sauros/tree/main)

Sauros is an interpreted language based on list execution, and under heavy development. 

Check out the [wiki](https://github.com/bosley/sauros/wiki) to see what has been implemented so far!

The goal of Sauros is to have an easy to use lisp-like scripting language with updated terminiology and features. 
There is a distant hope of having sauros also be compiled, but that will held off until a VM and feature lock is established in a base implementation. 

## Quick Sample

```
;
;  Generates a random number that the user tries to guess
;

; Use the built-in random module
[use "random"]

; Use the io module
[use "io"]

; Generate the number the user is going to guess
[var magic_number [random.uniform_int 0 100]]

; Loop until they get the correct number

[loop [var user_guess] [true] [] [block

   ; Get a number from the user
   [set user_guess [io.get_int "Guess a number> "]]

   ; Ensure that the user gave back a valid integer before 
   ; trying to do an integer comparison against the magic number
   [if [not [is_nil user_guess]] 
      [block
         ; Check if they got the number
         [if [== user_guess magic_number] [block
            [putln "You guessed the magic number!"]
            [exit 0]
         ]]

         ; If they didn't we keep going and check to see if they
         ; need to guess higher or lower
         [if [< user_guess magic_number] 
            [putln "Higher!"]
            [putln "Lower!"]
         ]
      ]

      ; Indicate to the user that their guess was invalid
      [putln "Invalid guess (please enter a number)"]
   ]
]]
```

## Building

Building Sauros is simple - there are no external dependencies aside from he library CPPUTest for unit tests (which can be disabled).

### Installing CPPUTest:

*Ubuntu/ Mac/ etc*

`./sauros/scripts/install_cpputest.sh`

### Building Sauros

```
git clone https://github.com/bosley/sauros.git

cd sauros/src

mkdir build_lib
cd build_lib
cmake ../libsauros
make -j5
sudo make install

cd ../
mkdir build_app
cd build_app
cmake ../app
make -j5
sudo make install
```

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

## Helping out

If you want to hop on Sauros development check out open issues or send me a message!

## Submitting Bugs

When submitting a bug please include the output from `sauros --system`. This will provide potentially helpful information about your system and the build of sauros that you were using when the bug popped up. In addition to this please list the following:

```
  Description of the issue:

  Code snippit that reproduces the issue:
```


<a target="_blank" href="https://icons8.com/icon/103610/lizard">Lizard icon by Icons8</a>

