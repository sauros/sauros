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

### Building Sauros

```
git clone https://github.com/sauros/sauros.git

cd sauros
mkdir build
cd build
cmake ../
make -j5
sudo make install
```

<a target="_blank" href="https://icons8.com/icon/103610/lizard">Lizard icon by Icons8</a>

