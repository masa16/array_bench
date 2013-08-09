#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <emmintrin.h>

typedef double dtype;

double gettimeofday_sec()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char* argv[])
{
    size_t i, j, n, m;
    double t, t0, ops;
    dtype x, y;
    char *a, *b, *c;
    char *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    __m128d d1, d2, d3;

    if (argc != 3) {
        fprintf(stderr,"usage: %s nsize nrepeat\n",argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);

    p1 = a = malloc(sizeof(dtype)*n);
    p2 = b = malloc(sizeof(dtype)*n);
    p3 = c = malloc(sizeof(dtype)*n);

    for (i=0; i<n; i++) {
        ((dtype*)a)[i] = i;
        ((dtype*)b)[i] = i;
    }
#if 0
    for (i=0; i<10; i++) {
        printf("%.2g ",((dtype*)a)[i]);
    }
    puts("");
#endif

#ifdef SSE2
    s1 = s2 = s3 = sizeof(dtype)*2;
#else
    s1 = s2 = s3 = sizeof(dtype);
#endif

    t0 = gettimeofday_sec();
    for (j=0; j<m; j++) {
        p1 = a;
        p2 = b;
        p3 = c;
#ifdef SSE2
        for (i=n; i-=2; ) {
            d1 = _mm_load_pd((dtype*)p1);   p1+=s1;
            d2 = _mm_load_pd((dtype*)p2);   p2+=s2;
            d3 = _mm_add_pd(d1,d2);
            _mm_store_pd((dtype*)p3,d3);    p3+=s3;
        }
#else
        for (i=n; i--; ) {
#if 1
            x = *(dtype*)p1; p1+=s1;
            y = *(dtype*)p2; p2+=s2;
            x = x+y;
            *(dtype*)p3 = x; p3+=s3;
#elif 0
            x = *(dtype*)p1;
            y = *(dtype*)p2;
            x = x+y;
            *(dtype*)p3 = x;
            p1+=s1;
            p2+=s2;
            p3+=s3;
#elif 0
            x = *(dtype*)(p1+i*s1);
            y = *(dtype*)(p2+i*s2);
            *(dtype*)(p3+i*s3) = x+y;
#elif 0
            ((dtype*)c)[i] = ((dtype*)a)[i] + ((dtype*)b)[i];
#endif
        }
#endif
    }
    t = gettimeofday_sec()-t0;
    ops = n * m * 1e-9 / t;
    printf("size=%d repeat=%d: %7.3f sec, %5.3f GFLOPS, %5.3f GB/s\n",
           n, m, t, ops, ops*24);
#if 0
    for (i=0; i<10; i++) {
        printf("%.2g ",((dtype*)c)[i]);
    }
    puts("");
#endif
    return 0;
}
