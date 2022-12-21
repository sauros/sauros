
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

[ ---------------------- ]
```

## Bosleys Lab - Server

The lab server is significantly slower than Midas so they are not directly comparable

```

20-December-2022

[ Release Build ]

-- https://github.com/bosley/parallel-hashmap

[Before hashmap modifications and general tweaking]

primality.sau >> averaged an execution time of  10.6058s ( 10605.8455ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.8258s ( 1825.8213ms )  after  50  executions

[after]

primality.sau >> averaged an execution time of  9.5126s ( 9512.6276ms )  after  50  executions
mandelbrot.sau >> averaged an execution time of  1.6811s ( 1681.1251ms )  after  50  executions

[ ---------------------- ]
```