#include <ruby.h>
#include <emmintrin.h>

VALUE cNArray;
typedef double dtype;
typedef struct RNArray {
    size_t   size;          // # of total elements
    char    *ptr;
} narray_t;

struct narray3 {
    narray_t *x, *y, *z;
    int repeat;
    VALUE stop;
};

#define GetNArray(obj,var)  Data_Get_Struct(obj, narray_t, var)


#ifdef HAVE_RB_THREAD_CALL_WITHOUT_GVL
static void
na_para_stop(void *ptr)
{
    struct narray3 *args = ptr;
    args->stop = Qtrue;
}
#endif

static void
na_free(narray_t* na)
{
    if (na->ptr != NULL) {
        xfree(na->ptr);
    }
    xfree(na);
}

static VALUE
na_s_allocate(VALUE klass)
{
    narray_t *na = ALLOC(narray_t);

    na->size = 0;
    na->ptr = NULL;
    return Data_Wrap_Struct(klass, 0, na_free, na);
}

static VALUE
na_initialize(VALUE self, VALUE vsize)
{
    size_t n, i;
    narray_t *na;
    double *a;

    Data_Get_Struct(self,narray_t,na);
    na->size = n = NUM2INT(vsize);
    a = ALLOC_N(double,n);
    na->ptr = (char*)a;
    for (i=0; i<n; i++) {
        a[i] = i;
    }
    return self;
}


static VALUE
na_print(VALUE self)
{
    size_t n, i;
    narray_t *na;
    double *a;

    Data_Get_Struct(self,narray_t,na);
    n = (na->size>8) ? 8 : na->size;
    a = (double*)(na->ptr);
    for (i=0; i<n; i++) {
        printf("%ld:%g, ",i,a[i]);
    }
    printf("..., ");
    n = na->size;
    for (i=na->size-4; i<n; i++) {
        printf("%ld:%g, ",i,a[i]);
    }
    printf("\n");
    return self;
}


static void *
add_sse2_unroll(void *ptr)
{
    size_t n;
    char *p1, *p2, *p3;
    struct narray3 *a = (struct narray3*)ptr;
    dtype x, y;
    size_t i, n4;
    ssize_t s1, s2, s3;
    __m128d d1, d2, d3;
    __m128d e1, e2, e3;
    char *q1, *q2, *q3;

    p1 = a->x->ptr;
    p2 = a->y->ptr;
    p3 = a->z->ptr;
    n =  a->x->size;

    s1 = s2 = s3 = sizeof(dtype);

    q1 = p1 + s1*2;
    q2 = p2 + s2*2;
    q3 = p3 + s3*2;

    s1 = s2 = s3 = sizeof(dtype)*4;

    n4 = 3;
    n4 = (n & ~n4) - 2;
        e1 = _mm_load_pd((dtype*)q1);
        q1+=s1;
        e2 = _mm_load_pd((dtype*)q2);
        q2+=s2;
        e3 = _mm_add_pd(e1,e2);
    for (i=2; i<n4; i+=4) {
        d1 = _mm_load_pd((dtype*)p1);
        p1+=s1;
        d2 = _mm_load_pd((dtype*)p2);
        p2+=s2;
        d3 = _mm_add_pd(d1,d2);
        _mm_store_pd((dtype*)q3,e3);
        q3+=s3;
        e1 = _mm_load_pd((dtype*)q1);
        q1+=s1;
        e2 = _mm_load_pd((dtype*)q2);
        q2+=s2;
        e3 = _mm_add_pd(e1,e2);
        _mm_store_pd((dtype*)p3,d3);
        p3+=s3;
    }
    _mm_store_pd((dtype*)q3,e3);

    for (; i<n; i++) {
        x = *(dtype*)p1; p1+=s1;
        y = *(dtype*)p2; p2+=s2;
        x = x+y;
        *(dtype*)p3 = x; p3+=s3;
    }
    return 0;
}

static void *
add_sse2(void *ptr)
{
    size_t n;
    char *p1, *p2, *p3;
    struct narray3 *a = (struct narray3*)ptr;
    dtype x, y;
    size_t i, n2;
    ssize_t s1, s2, s3;
    __m128d d1, d2, d3;

    p1 = a->x->ptr;
    p2 = a->y->ptr;
    p3 = a->z->ptr;
    n =  a->x->size;

    s1 = s2 = s3 = sizeof(dtype)*2;

    n2 = 1;
    n2 = n & ~n2;
    for (i=0; i<n2; i+=2) {
        d1 = _mm_load_pd((dtype*)p1);
        p1+=s1;
        d2 = _mm_load_pd((dtype*)p2);
        p2+=s2;
        d3 = _mm_add_pd(d1,d2);
        _mm_store_pd((dtype*)p3,d3);
        p3+=s3;
    }

    for (; i<n; i++) {
        x = *(dtype*)p1; p1+=s1;
        y = *(dtype*)p2; p2+=s2;
        x = x+y;
        *(dtype*)p3 = x; p3+=s3;
    }
    return 0;
}

static void *
add_basic(void *ptr)
{
    size_t n;
    dtype x, y;
    char *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    struct narray3 *a = (struct narray3*)ptr;
    s1 = s2 = s3 = sizeof(dtype);

    p1 = a->x->ptr;
    p2 = a->y->ptr;
    p3 = a->z->ptr;
    n =  a->x->size;

    for (; n--;) {
        x = *(dtype*)p1;    p1+=s1;
        y = *(dtype*)p2;    p2+=s2;
        *(dtype*)p3 = x+y;  p3+=s3;
    }
    return 0;
}

static void *
add_unroll(void *ptr)
{
    size_t n;
    dtype x1, y1, z1;
    dtype x2, y2, z2;
    char *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    struct narray3 *a = (struct narray3*)ptr;
    s1 = s2 = s3 = sizeof(dtype);

    p1 = a->x->ptr;
    p2 = a->y->ptr;
    p3 = a->z->ptr;
    n =  a->x->size;

    if (n==0) return 0;
    n--;
    if (n & 1) {
        x1 = *(dtype*)p1;    p1+=s1;
        y1 = *(dtype*)p2;    p2+=s2;
        z1 = x1 + y1;
    }
    x2 = *(dtype*)p1;    p1+=s1;
    y2 = *(dtype*)p2;    p2+=s2;
    z2 = x2 + y2;
    if (n & 1) {
        n--;
        *(dtype*)p3 = z1;    p3+=s3;
    }
    for (; n;) {
        n -= 2;
        x1 = *(dtype*)p1;    p1+=s1;
        y1 = *(dtype*)p2;    p2+=s2;
        z1 = x1 + y1;
        *(dtype*)p3 = z2;    p3+=s3;
        x2 = *(dtype*)p1;    p1+=s1;
        y2 = *(dtype*)p2;    p2+=s2;
        z2 = x2 + y2;
        *(dtype*)p3 = z1;    p3+=s3;
    }
    *(dtype*)p3 = z2;    p3+=s3;
    return 0;
}


static VALUE
na_add(VALUE self, VALUE other, VALUE repeat, void*(*func)(void *))
{
    volatile VALUE result;
    struct narray3 a;
    int i;

    Data_Get_Struct(self,narray_t,a.x);
    Data_Get_Struct(other,narray_t,a.y);
    if (a.x->size != a.y->size) {
        rb_raise(rb_eArgError,"size mismatch");
    }
    result = na_s_allocate(cNArray);
    Data_Get_Struct(result,narray_t,a.z);
    a.z->size = a.x->size;
    a.z->ptr = ALLOC_N(char,a.z->size*sizeof(double));
    a.stop = Qfalse;
    a.repeat = NUM2INT(repeat);
    for (i=0; i<a.repeat; i++) {
#ifdef HAVE_RB_THREAD_CALL_WITHOUT_GVL
        rb_thread_call_without_gvl(func, &a, na_para_stop, &a);
#else
        func(&a);
#endif
    }
    return result;
}


static VALUE
na_add_basic(VALUE self, VALUE other, VALUE repeat)
{ return na_add(self,other,repeat,add_basic); }

static VALUE
na_add_unroll(VALUE self, VALUE other, VALUE repeat)
{ return na_add(self,other,repeat,add_unroll); }

static VALUE
na_add_sse2(VALUE self, VALUE other, VALUE repeat)
{ return na_add(self,other,repeat,add_sse2); }

static VALUE
na_add_sse2_unroll(VALUE self, VALUE other, VALUE repeat)
{ return na_add(self,other,repeat,add_sse2_unroll); }


/* initialization of NArray Class */
void
Init_narray()
{
    /* define NArray class */
    cNArray = rb_define_class("NArray", rb_cObject);

    rb_define_alloc_func(cNArray, na_s_allocate);
    rb_define_method(cNArray, "initialize", na_initialize, 1);
    rb_define_method(cNArray, "add", na_add_basic, 2);
    rb_define_method(cNArray, "add_unroll", na_add_unroll, 2);
    rb_define_method(cNArray, "add_sse2", na_add_sse2, 2);
    rb_define_method(cNArray, "add_sse2_unroll", na_add_sse2_unroll, 2);
    rb_define_method(cNArray, "print", na_print, 0);
}
