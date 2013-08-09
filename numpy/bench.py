import time
from numpy import *

params = [
[100,10000000],
[1000,1000000],
[10000,100000],
[100000,10000],
[1000000,1000],
[10000000,100],
[50000000,20],
[10000000,100],
[1000000,1000],
[100000,10000],
[10000,100000],
[1000,1000000],
[100,10000000],
]

for p in params:
  n = p[0]
  m = p[1]
  a = arange(n,dtype=float64)
  b = arange(n,dtype=float64)
  start = time.time()
  for i in range(m):
      c = a + b
  stop = time.time()
  t = stop-start
  ops = n * m * 1e-9 / t
  print("size=%d repeat=%d: %7.3f sec, %5.3f GFLOPS, %5.3f GB/s" % (n,m,t,ops,ops*24))
