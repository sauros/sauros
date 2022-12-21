
```
[ Release Build ]
[ ---- Midas Server ---- ]

19-September-2022

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
[ Release Build ]

20-December-2022

-- https://github.com/bosley/parallel-hashmap

[Before prallel hashmap]

primality
   100,000 Primes
      0 - 9.13s user 0.03s system 99% cpu 9.195 total
      1 - 11.16s user 0.05s system 99% cpu 11.279 total
      2 - 9.38s user 0.05s system 98% cpu 9.524 total
      3 - 10.30s user 0.04s system 99% cpu 10.428 total
      4 - 10.68s user 0.04s system 99% cpu 10.802 total

mandelbrot
      0 - 1.62s user 0.02s system 99% cpu 1.647 total
      1 - 1.97s user 0.02s system 99% cpu 2.017 total
      2 - 2.37s user 0.00s system 99% cpu 2.386 total
      3 - 1.54s user 0.01s system 98% cpu 1.565 total
      4 - 2.27s user 0.02s system 99% cpu 2.303 total

[Flat parallel hashmap (environment_c only)]

primality
   100,000 Primes
      0 - 10.39s user 0.03s system 99% cpu 10.472 total
      1 - 9.84s user 0.03s system 99% cpu 9.964 total
      2 - 10.23s user 0.02s system 99% cpu 10.358 total
      3 - 11.80s user 0.05s system 98% cpu 12.043 total
      4 - 10.89s user 0.02s system 99% cpu 10.974 total

mandelbrot
      0 - 1.62s user 0.01s system 98% cpu 1.653 total
      1 - 2.80s user 0.06s system 96% cpu 2.965 total
      2 - 1.91s user 0.01s system 98% cpu 1.938 total
      3 - 1.88s user 0.01s system 98% cpu 1.912 total
      4 - 2.02s user 0.01s system 98% cpu 2.049 total

[Flat parallel hashmap (all maps)]

primality
   100,000 Primes
      0 - 9.85s user 0.03s system 99% cpu 9.938 total
      1 - 11.10s user 0.03s system 99% cpu 11.191 total
      2 - 11.26s user 0.01s system 99% cpu 11.316 total
      3 - 11.56s user 0.02s system 99% cpu 11.681 total
      4 - 11.53s user 0.02s system 99% cpu 11.651 total

mandelbrot
      0 - 1.61s user 0.02s system 98% cpu 1.652 total
      1 - 2.17s user 0.02s system 98% cpu 2.207 total
      2 - 2.34s user 0.01s system 98% cpu 2.370 total
      3 - 1.90s user 0.02s system 97% cpu 1.961 total
      4 - 1.61s user 0.01s system 99% cpu 1.638 total

[Node parallel hashmap (all maps)]

primality
   100,000 Primes
      0 - 9.97s user 0.02s system 99% cpu 10.041 total
      1 - 10.55s user 0.06s system 98% cpu 10.737 total
      2 - 9.45s user 0.01s system 99% cpu 9.559 total
      3 - 9.43s user 0.05s system 99% cpu 9.572 total
      4 - 10.62s user 0.03s system 99% cpu 10.728 total

mandelbrot
      0 - 2.25s user 0.01s system 98% cpu 2.304 total
      1 - 2.06s user 0.02s system 98% cpu 2.110 total
      2 - 2.24s user 0.01s system 99% cpu 2.271 total
      3 - 1.91s user 0.01s system 98% cpu 1.950 total
      4 - 1.68s user 0.02s system 98% cpu 1.725 total

[Node hashmap (all maps)]

primality
   100,000 Primes
      0 - 9.10s user 0.02s system 99% cpu 9.162 total
      1 - 11.07s user 0.05s system 98% cpu 11.239 total
      2 - 10.74s user 0.04s system 99% cpu 10.836 total
      3 - 9.64s user 0.02s system 99% cpu 9.680 total
      4 - 10.22s user 0.03s system 98% cpu 10.382 total

mandelbrot
      0 - 1.92s user 0.01s system 98% cpu 1.968 total
      1 - 2.32s user 0.01s system 99% cpu 2.355 total
      2 - 2.06s user 0.02s system 98% cpu 2.101 total
      3 - 1.64s user 0.01s system 99% cpu 1.658 total
      4 - 2.03s user 0.02s system 99% cpu 2.066 total

[Flat hashmap (all maps)]

primality
   100,000 Primes
      0 - 10.14s user 0.03s system 98% cpu 10.319 total
      1 - 10.41s user 0.00s system 99% cpu 10.494 total
      2 - 11.57s user 0.05s system 98% cpu 11.764 total
      3 - 9.54s user 0.03s system 99% cpu 9.620 total
      4 - 10.22s user 0.02s system 99% cpu 10.275 total

mandelbrot
      0 - 2.10s user 0.00s system 98% cpu 2.135 total
      1 - 2.21s user 0.01s system 98% cpu 2.247 total
      2 - 2.40s user 0.03s system 98% cpu 2.463 total
      3 - 1.63s user 0.00s system 99% cpu 1.649 total
      4 - 1.90s user 0.01s system 98% cpu 1.951 total

[ ---------------------- ]
```