// Force unistd to be included first, since if it's included later we'll get a compile error.
#include <unistd.h>

long __syscall1(long n, long a);
long __syscall2(long n, long a, long b);
long __syscall3(long n, long a, long b, long c);
long __syscall4(long n, long a, long b, long c, long d);
long __syscall6(long n, long a, long b, long c, long d, long e, long f);
