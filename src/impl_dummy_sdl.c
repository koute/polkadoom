#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>

#include <SDL.h>

void SDL_AssertionsQuit(void) {
}

void SDL_ClearHints(void) {
}

void * SDL_malloc(size_t size) {
    return malloc(size);
}

void SDL_free(void * p) {
    return free(p);
}

void * SDL_calloc(size_t n, size_t s) {
    return calloc(n, s);
}

void * SDL_realloc(void * p, size_t s) {
    return realloc(p, s);
}

void * SDL_memcpy(void * d, const void * s, size_t n) {
    return memcpy(d, s, n);
}

void * SDL_memset(void * s, int c, size_t n) {
    return memset(s, c, n);
}

int SDL_strcmp(const char * s1, const char * s2) {
    return strcmp(s1, s2);
}

int SDL_strcasecmp(const char * s1, const char * s2) {
    return strcasecmp(s1, s2);
}

int SDL_strncasecmp(const char * s1, const char * s2, size_t n) {
    return strncasecmp(s1, s2, n);
}

long SDL_strtol(const char * p, char ** e, int base) {
    return strtol(p, e, base);
}

char * SDL_strdup(const char * s) {
    return strdup(s);
}

size_t SDL_strlen(const char * s) {
    return strlen(s);
}

char * SDL_strtokr(char * s, const char * d, char ** v) {
    return strtok_r(s, d, v);
}

int SDL_memcmp(const void * s1, const void * s2, size_t n) {
    return memcmp(s1, s2, n);
}

size_t SDL_strlcpy(char * d, const char * s, size_t n) {
    return strlcpy(d, s, n);
}

char * SDL_strrchr(const char * s, int c) {
    return strrchr(s, c);
}

int SDL_atoi(const char * p) {
    return atoi(p);
}

int SDL_isdigit(int c) {
    return isdigit(c);
}

double SDL_atof(const char *nptr) {
    return atof(nptr);
}

float SDL_floorf(float x) {
    return floorf(x);
}

char * SDL_getenv(const char *name) {
    return getenv(name);
}

char * SDL_strchr(const char *s, int c) {
    return strchr(s, c);
}

double SDL_strtod(const char * nptr, char ** endptr) {
    return strtod(nptr, endptr);
}

long long SDL_strtoll(const char * nptr, char ** endptr, int base) {
    return strtoll(nptr, endptr, base);
}

char * SDL_iconv_string(const char *tocode, const char *fromcode, const char *inbuf, size_t inbytesleft) {
    return NULL;
}

int SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...) {
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);
    return ret;
}

int SDL_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap) {
    return vsnprintf(text, maxlen, fmt, ap);
}

double SDL_ceil(double x) {
    return ceil(x);
}

void * SDL_memmove(void * d, const void * s, size_t n) {
    return memmove(d, s, n);
}

int SDL_sscanf(const char *s, const char *fmt, ...) {
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsscanf(s, fmt, ap);
    va_end(ap);
    return ret;
}

const char * SDL_GetHint(const char *name) {
    return NULL;
}

SDL_bool SDL_GetHintBoolean(const char *name, SDL_bool default_value) {
    return default_value;
}

SDL_bool SDL_AtomicCAS(SDL_atomic_t *a, int oldval, int newval) {
    SDL_bool retval = SDL_FALSE;

    if (a->value == oldval) {
        a->value = newval;
        retval = SDL_TRUE;
    }

    return retval;
}

int SDL_AtomicAdd(SDL_atomic_t *a, int v)
{
    int value;
    do {
        value = a->value;
    } while (!SDL_AtomicCAS(a, value, (value + v)));
    return value;
}

int SDL_AtomicGet(SDL_atomic_t *a) {
    return a->value;
}

int SDL_AtomicSet(SDL_atomic_t *a, int v) {
    int old = a->value;
    a->value = v;
    return old;
}

/*
static Uint32 SDL_INIT = 0;

int SDL_Init(Uint32 flags) {
    return SDL_InitSubSystem(flags);
}

Uint32 SDL_WasInit(Uint32 flags) {
    if (flags == 0) {
        return SDL_INIT;
    } else {
        return SDL_INIT & flags;
    }
}

int SDL_InitSubSystem(Uint32 flags) {
    SDL_INIT = SDL_INIT | flags;
    return 0;
}

void SDL_QuitSubSystem(Uint32 flags) {
}
*/

int SDL_EventsInit(void) {
    return 0;
}

void SDL_EventsQuit(void) {
}

Uint8 SDL_EventState(Uint32 type, int state) {
    return SDL_DISABLE;
}

int SDL_PushEvent(SDL_Event * event) {
    return 0;
}
