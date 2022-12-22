results = []
def is_prime(n):
   if n == 2 or n == 3:
      results.append(n)
      return
   if n > 1 and n % 2 and n% 3:
      i = 5
      do_pass = 1
      while do_pass and (i * i <= n):
         if n % i == 0:
            do_pass = 0
         if n % (i + 2) == 0:
            do_pass = 0
         i = 6 + i
      if do_pass:
         results.append(n)

for x in range(0, 100000):
   is_prime(x)

for x in results:
   print(x, end=" ")