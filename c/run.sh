#! /bin/sh
./bench-basic 100 10000000
./bench-sse2  100 10000000

./bench-basic 1000 1000000
./bench-sse2  1000 1000000

./bench-basic 10000 100000
./bench-sse2  10000 100000

./bench-basic 100000 10000
./bench-sse2  100000 10000

./bench-basic 1000000 1000
./bench-sse2  1000000 1000

./bench-basic 10000000 100
./bench-sse2  10000000 100

./bench-basic 50000000 20
./bench-sse2  50000000 20

./bench-basic 1000000 1000
./bench-sse2  1000000 1000

./bench-basic 100000 10000
./bench-sse2  100000 10000

./bench-basic 10000 100000
./bench-sse2  10000 100000

./bench-basic 1000 1000000
./bench-sse2  1000 1000000

./bench-basic 100 10000000
./bench-sse2  100 10000000
