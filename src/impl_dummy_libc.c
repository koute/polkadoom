#include <stdlib.h>
#include <pthread.h>

__attribute__((noreturn)) void exit(int status) {
    abort();
}

__attribute__((noreturn)) void _exit(int status) {
    abort();
}

int __aio_close(int fd) {
    return 0;
}

static unsigned char TLS[256];

unsigned char * __get_tp() {
    return TLS;
}

int system(const char *command) {
    return -1;
}

int putenv(char *s) {
    return 0;
}

char * getenv(const char * name) {
    return NULL;
}

int __cxa_atexit(void (*handler)(void *), void * a, void * b) {
    return 0;
}

void __cxa_pure_virtual(void) {
    abort();
}

void * aligned_alloc(size_t alignment, size_t size) {
    return malloc(size);
}

int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr) {
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    return 0;
}
