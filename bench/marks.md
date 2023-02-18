## Bosleys Lab - Server

The lab server is significantly slower than Midas so they are not directly comparable

### 20-December-2022

Change out hashmap from `unordered_map` to :
https://github.com/bosley/parallel-hashmap

```
[before]

primality.saur >> averaged an execution time of  10.6058s ( 10605.8455ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  1.8258s ( 1825.8213ms )  after  50  executions

[after]

primality.saur >> averaged an execution time of  9.5126s ( 9512.6276ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  1.6811s ( 1681.1251ms )  after  50  executions
```

### 22-December-22

Update the usage from `cell_c` in every cell to `std::shared_ptr<cell_c>` aka `cell_ptr`. The idea is to minimize size
of temporary cells being passed around.


`cell_c` size (bytes): 144

`cell_ptr` size (bytes): 16

**Results:**
```
primality.saur >> averaged an execution time of  4.0192s ( 4019.1865ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  1.0959s ( 1095.9398ms )  after  50  executions
```

As can be seen the primality test benifited greatly from this optimization (5.5 SECONDS faster) . The mandelbrot test did benifit (.6 SECONDS faster), but not to the magnitude of the primality test.

Clearly, the primality test has more operations that result in the copying of a cell structure.


### 3-January-2022

Updates over the last week that include the code to trace the origin of cells slowed down the bench mark:

```
primality.saur >> averaged an execution time of  5.5556s ( 5555.6415ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  1.3835s ( 1383.4991ms )  after  50  executions
```

Converting `location_s` to a raw pointer cut the size of the cell down 8 bytes. The total amount of memory the program will eat up is +8 bytes per cell, as the 16 used by location still exist, and now there is a pointer to a location, but despite the location
having to be cloned into memory every time, and having cells manage the location memory the new bench is:

```
primality.saur >> averaged an execution time of  4.8319s ( 4831.8663ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  1.2559s ( 1255.8673ms )  after  50  executions
```


# Changing of benchmark platform:

All benchmarks are now running on `sushi` - Current code runs : 

```
primality.saur >> averaged an execution time of  0.8564s ( 856.4176ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.2326s ( 232.6051ms )  after  50  execution
```

### 8-January-2022

Added new cell variant type and async, updated benchs to include nsieve set to 4

```
primality.saur >> averaged an execution time of  0.8979s ( 897.8553ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.2604s ( 260.4496ms )  after  50  executions
4_nsieve.saur >> averaged an execution time of  0.7299s ( 729.903ms )  after  50  executions
```

### 10-January-2022

Created cell type for boxed variable to ensure that we don't have to scan every single
symbol for a `.` at run time.
```
primality.saur >> averaged an execution time of  0.8108s ( 810.7506ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.244s ( 244.0041ms )  after  50  executions
4sieve.saur >> averaged an execution time of  0.6784s ( 678.36ms )  after  50  executions
```

## 22-January-2022

Update to cells to use a union to store the underlying data together in an int64_t, 
double, and a std::string pointer. This means that we can directly work with values
rather than relying on string conversions (ew)

It can be seen by the results of the benchmarks that the 2 benchmarks that utilize a lot of 
number manipulation have improved pretty significantly
```
primality.saur >> averaged an execution time of  0.6962s ( 696.1787ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.1345s ( 134.5026ms )  after  50  executions
4sieve.saur >> averaged an execution time of  0.656s ( 656.0264ms )  after  50  executions
```

## 28-January-2022

I got a new laptop that I will be using for development so I am placing the
new benchmarks here. Its an older Thinkpad T490 so I suspect it will be slower
than the asus tuf that was being used before.

```
primality.saur >> averaged an execution time of  1.3249s ( 1324.9109ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.2413s ( 241.2618ms )  after  50  executions
4sieve.saur >> averaged an execution time of  1.1085s ( 1108.4888ms )  after  50  executions
```

.. yeah its a bit slower 

## 18-Feb-2023

A little bit of work to try to optimize things

```
primality.saur >> averaged an execution time of  1.1859s ( 1185.9115ms )  after  50  executions
mandelbrot.saur >> averaged an execution time of  0.2254s ( 225.4281ms )  after  50  executions
4sieve.saur >> averaged an execution time of  1.0611s ( 1061.1067ms )  after  50  executions
```