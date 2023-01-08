
```
[ Release Build ]
[ ---- Midas Server ---- ]

19-September-2022

[Before hashmap modifications and general tweaking]

primality
   100,000 Primes
      0 - 5.38s user 0.00s system 99% cpu 5.384 total
      1 - 5.35s user 0.01s system 99% cpu 5.360 total
      2 - 5.35s user 0.01s system 99% cpu 5.360 total
      3 - 5.41s user 0.00s system 99% cpu 5.408 total
      4 - 5.38s user 0.00s system 99% cpu 5.388 total

mandelbrot
      0 - 0.86s user 0.00s system 99% cpu 0.857 total
      1 - 0.85s user 0.00s system 99% cpu 0.855 total
      2 - 0.86s user 0.00s system 99% cpu 0.865 total
      3 - 0.85s user 0.00s system 99% cpu 0.859 total
      4 - 0.85s user 0.00s system 99% cpu 0.855 total

[new metrics as of first map optimization work]

primality.sau >> averaged an execution time of  4.8125s ( 4812.522ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  0.7721s ( 772.0777ms )  after  50  executions

[ ---------------------- ]
```

## Bosleys Lab - Server

The lab server is significantly slower than Midas so they are not directly comparable

### 20-December-2022

Change out hashmap from `unordered_map` to :
https://github.com/bosley/parallel-hashmap

```
[before]

primality.sau >> averaged an execution time of  10.6058s ( 10605.8455ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.8258s ( 1825.8213ms )  after  50  executions

[after]

primality.sau >> averaged an execution time of  9.5126s ( 9512.6276ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.6811s ( 1681.1251ms )  after  50  executions
```

### 22-December-22

Update the usage from `cell_c` in every cell to `std::shared_ptr<cell_c>` aka `cell_ptr`. The idea is to minimize size
of temporary cells being passed around.


`cell_c` size (bytes): 144

`cell_ptr` size (bytes): 16

**Results:**
```
primality.sau >> averaged an execution time of  4.0192s ( 4019.1865ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.0959s ( 1095.9398ms )  after  50  executions
```

As can be seen the primality test benifited greatly from this optimization (5.5 SECONDS faster) . The mandelbrot test did benifit (.6 SECONDS faster), but not to the magnitude of the primality test.

Clearly, the primality test has more operations that result in the copying of a cell structure.


### 3-January-2022

Updates over the last week that include the code to trace the origin of cells slowed down the bench mark:

primality.sau >> averaged an execution time of  5.5556s ( 5555.6415ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.3835s ( 1383.4991ms )  after  50  executions

Converting `location_s` to a raw pointer cut the size of the cell down 8 bytes. The total amount of memory the program will eat up is +8 bytes per cell, as the 16 used by location still exist, and now there is a pointer to a location, but despite the location
having to be cloned into memory every time, and having cells manage the location memory the new bench is:

primality.sau >> averaged an execution time of  4.8319s ( 4831.8663ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.2559s ( 1255.8673ms )  after  50  executions


# Changing of benchmark platform:

All benchmarks are now running on `sushi` - Current code runs : 

primality.sau >> averaged an execution time of  0.8564s ( 856.4176ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  0.2326s ( 232.6051ms )  after  50  execution

### 8-January-2022

Added new cell variant type and async, updated benchs to include nsieve set to 4

primality.sau >> averaged an execution time of  0.8979s ( 897.8553ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  0.2604s ( 260.4496ms )  after  50  executions
4_nsieve.sau >> averaged an execution time of  0.7299s ( 729.903ms )  after  50  executions