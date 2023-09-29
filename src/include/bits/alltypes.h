#ifndef ALLTYPES_H
#define ALLTYPES_H

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __USE_TIME_BITS64 1

#if __riscv_xlen == 32

#define _Addr int
#define _Int64 long long
#define _Reg int
#define __BYTE_ORDER __LITTLE_ENDIAN
#define __LONG_MAX 0xffffffff

#else

#define _Addr long
#define _Int64 long
#define _Reg long
#define __BYTE_ORDER __LITTLE_ENDIAN
#define __LONG_MAX 0xffffffffffffffffl

#endif

#define MINSIGSTKSZ 4096

#define weak __attribute__((__weak__))
#define hidden __attribute__((__visibility__("hidden")))
#define weak_alias(old, new) \
    extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

#define a_ll a_ll
#define a_sc a_sc

static inline int a_sc(volatile int *p, int v) {
    int r = *p;
    *p = v;
    return r;
}

static inline int a_ll(volatile int *p) {
    return *p;
}

#ifdef wchar_t
#undef wchar_t
#endif

typedef unsigned wchar_t;
typedef float float_t;
typedef double double_t;

typedef long long max_align_t;

typedef unsigned _Addr size_t;
typedef unsigned _Addr uintptr_t;
typedef _Addr ptrdiff_t;
typedef _Addr ssize_t;
typedef _Addr intptr_t;
typedef _Addr regoff_t;
typedef _Reg register_t;
typedef _Int64 time_t;
typedef _Int64 suseconds_t;

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef signed _Int64   int64_t;
typedef signed _Int64   intmax_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned _Int64 uint64_t;
typedef unsigned _Int64 u_int64_t;
typedef unsigned _Int64 uintmax_t;

typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;

typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;

typedef unsigned mode_t;
typedef unsigned _Reg nlink_t;
typedef _Int64 off_t;
typedef unsigned _Int64 ino_t;
typedef unsigned _Int64 dev_t;
typedef long blksize_t;
typedef _Int64 blkcnt_t;
typedef unsigned _Int64 fsblkcnt_t;
typedef unsigned _Int64 fsfilcnt_t;

typedef unsigned wint_t;
typedef unsigned long wctype_t;

typedef void * timer_t;
typedef int clockid_t;
typedef long clock_t;
struct timeval { time_t tv_sec; suseconds_t tv_usec; };
struct timespec { time_t tv_sec; int :8*(sizeof(time_t)-sizeof(long))*(__BYTE_ORDER==4321); long tv_nsec; int :8*(sizeof(time_t)-sizeof(long))*(__BYTE_ORDER!=4321); };

typedef int pid_t;
typedef unsigned id_t;
typedef unsigned uid_t;
typedef unsigned gid_t;
typedef int key_t;
typedef unsigned useconds_t;

#ifdef __cplusplus
typedef unsigned long pthread_t;
#else
typedef struct __pthread * pthread_t;
#endif
typedef int pthread_once_t;
typedef unsigned pthread_key_t;
typedef int pthread_spinlock_t;
typedef struct { unsigned __attr; } pthread_mutexattr_t;
typedef struct { unsigned __attr; } pthread_condattr_t;
typedef struct { unsigned __attr; } pthread_barrierattr_t;
typedef struct { unsigned __attr[2]; } pthread_rwlockattr_t;

typedef struct _IO_FILE FILE;

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;

typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;

typedef struct __locale_struct * locale_t;

typedef struct __sigset_t { unsigned long __bits[128/sizeof(long)]; } sigset_t;

struct iovec { void *iov_base; size_t iov_len; };

struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };

typedef unsigned socklen_t;
typedef unsigned short sa_family_t;

typedef struct { union { int __i[sizeof(long)==8?14:9]; volatile int __vi[sizeof(long)==8?14:9]; unsigned long __s[sizeof(long)==8?7:9]; } __u; } pthread_attr_t;
typedef struct { union { int __i[sizeof(long)==8?10:6]; volatile int __vi[sizeof(long)==8?10:6]; volatile void *volatile __p[sizeof(long)==8?5:6]; } __u; } pthread_mutex_t;
typedef struct { union { int __i[sizeof(long)==8?10:6]; volatile int __vi[sizeof(long)==8?10:6]; volatile void *volatile __p[sizeof(long)==8?5:6]; } __u; } mtx_t;
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12*sizeof(int)/sizeof(void*)]; } __u; } pthread_cond_t;
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12*sizeof(int)/sizeof(void*)]; } __u; } cnd_t;
typedef struct { union { int __i[sizeof(long)==8?14:8]; volatile int __vi[sizeof(long)==8?14:8]; void *__p[sizeof(long)==8?7:8]; } __u; } pthread_rwlock_t;
typedef struct { union { int __i[sizeof(long)==8?8:5]; volatile int __vi[sizeof(long)==8?8:5]; void *__p[sizeof(long)==8?4:5]; } __u; } pthread_barrier_t;

#undef _Addr
#undef _Int64
#undef _Reg

#endif
