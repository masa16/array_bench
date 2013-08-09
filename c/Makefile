all: bench-basic bench-sse2

CFLAGS=-g -O3 -funroll-loops

bench-basic: bench.c
	gcc $(CFLAGS) bench.c -o bench-basic

bench-sse2: bench.c
	gcc $(CFLAGS) bench.c -DSSE2 -msse2 -o bench-sse2

clean:
	rm -f bench-basic bench-sse2
