#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <math.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <SDL.h>
#include "../libs/SDL/src/SDL_internal.h"
#include "../libs/SDL/src/audio/SDL_audio_c.h"
#include "../libs/SDL/src/audio/SDL_sysaudio.h"

#ifdef __riscv

#include "polkavm_guest.h"

POLKAVM_IMPORT(void, ext_output_video, long, size_t, size_t);
POLKAVM_IMPORT(void, ext_output_audio, long, size_t);
POLKAVM_IMPORT(long, ext_rom_size);
POLKAVM_IMPORT(void, ext_rom_read, long, size_t, size_t);
POLKAVM_IMPORT(long, ext_stdout, long, size_t);

#endif

static void flush_stdio() {
    fflush(stdout);
    fflush(stderr);
}

__attribute__((noreturn)) void abort(void) {
    fprintf(stderr, "\nabort() called!\n");
    flush_stdio();

    POLKAVM_TRAP();
}

#define MEMORY_SIZE (16 * 1024 * 1024)

static unsigned char _MEMORY[MEMORY_SIZE];
static unsigned long _SBRK = 0;

void *sbrk(intptr_t inc) {
    if (_SBRK + inc > MEMORY_SIZE) {
        errno = ENOMEM;
        return -1;
    }

    void * p = (void *)(_MEMORY + _SBRK);
    _SBRK += inc;
    return p;
}

long __syscall_cp(long n) {
    if (n == SYS_close) {
        return 0;
    } else {
        fprintf(stderr, "WARN: unhandled syscall(0): %li\n", n);
        flush_stdio();
        return -ENOSYS;
    }
}

long __syscall1(long n, long a0) {
    switch (n) {
        case SYS_close:
        case SYS_unlink:
        {
            return 0;
        }
        default:
        {
            fprintf(stderr, "WARN: unhandled syscall(1): %li\n", n);
            flush_stdio();
            return -ENOSYS;
        }
    }
}

long __syscall2(long n, long a0, long a1) {
    switch (n) {
        case SYS_mkdir:
        case SYS_rename:
        {
            return 0;
        }
        default:
        {
            fprintf(stderr, "WARN: unhandled syscall(2): %li\n", n);
            flush_stdio();
            return -ENOSYS;
        }
    }
}

#define FD_STDOUT 1
#define FD_STDERR 2
#define FD_WAD 3
#define FD_MIDI 4
#define FD_DUMMY 10

static long ROM_SIZE = 0;
static long ROM_OFFSET = 0;

static char * MIDI = 0;
static long MIDI_OFFSET = 0;
static long MIDI_SIZE = 0;
static long MIDI_CAPACITY = 0;

static long write_midi(long buffer, size_t length) {
    long tail = MIDI_OFFSET + length;
    if (tail > MIDI_CAPACITY) {
        if (MIDI_CAPACITY == 0) {
            MIDI_CAPACITY = 1024;
        }

        while (tail > MIDI_CAPACITY) {
            MIDI_CAPACITY = MIDI_CAPACITY * 2;
        }
        MIDI = realloc(MIDI, MIDI_CAPACITY);
    }

    memcpy(MIDI + MIDI_OFFSET, buffer, length);
    MIDI_OFFSET += length;
    if (MIDI_OFFSET > MIDI_SIZE) {
        MIDI_SIZE = MIDI_OFFSET;
    }

    return length;
}

static long write_dummy(long buffer, size_t length) {
    return length;
}

long __syscall3(long n, long a0, long a1, long a2) {
    switch (n) {
        case SYS_open:
        {
            const char * filename = a0;
            fprintf(stderr, "\nsys_open: '%s'\n", filename);
            flush_stdio();

            if (!strcmp(filename, "doom1.wad")) {
                fprintf(stderr, "sys_open: fetching rom size...\n");
                flush_stdio();

                ROM_SIZE = ext_rom_size();
                fprintf(stderr, "sys_open: fetched rom size: %li\n", ROM_SIZE);
                flush_stdio();

                return FD_WAD;
            } else if (!strcmp(filename, "/tmp/doom.mid")) {
                MIDI_OFFSET = 0;
                return FD_MIDI;
            } else if (!strcmp(filename, "./.savegame/temp.dsg")) {
                return FD_DUMMY;
            } else {
                return -ENOENT;
            }
        }
        case SYS_lseek:
        {
            long * offset = 0;
            long * size = 0;
            if (a0 == FD_WAD) {
                offset = &ROM_OFFSET;
                size = &ROM_SIZE;
            } else if (a0 == FD_MIDI) {
                offset = &MIDI_OFFSET;
                size = &MIDI_SIZE;
            } else if (a0 == FD_DUMMY) {
                return 0;
            } else {
                fprintf(stderr, "WARN: lseek on an unknown FD: %li\n", a0);
                flush_stdio();

                return -EBADF;
            }

            switch (a2) {
                case SEEK_SET:
                    *offset = a1;
                    break;
                case SEEK_CUR:
                    *offset += a1;
                    break;
                case SEEK_END:
                    *offset = *size + a1;
                    break;
                default:
                    return -EINVAL;
            }

            return *offset;
        }
        case SYS_ioctl:
        {
            return -ENOSYS;
        }
        case SYS_readv:
        {
            long * fd_offset = 0;
            long * fd_size = 0;
            if (a0 == FD_WAD) {
                fd_offset = &ROM_OFFSET;
                fd_size = &ROM_SIZE;
            } else if (a0 == FD_MIDI) {
                fd_offset = &MIDI_OFFSET;
                fd_size = &MIDI_SIZE;
            } else if (a0 == FD_DUMMY) {
                return 0;
            } else {
                fprintf(stderr, "WARN: read from an unknown FD: %li\n", a0);
                flush_stdio();

                return -EBADF;
            }

            const struct iovec *iov = (const struct iovec *)a1;
            long bytes_read = 0;
            for (long i = 0; i < a2; ++i) {
                long remaining = *fd_size - *fd_offset;
                long length = iov[i].iov_len;
                if (remaining < length) {
                    length = remaining;
                }
                if (length == 0) {
                    break;
                }

                if (a0 == FD_WAD) {
                    ext_rom_read(iov[i].iov_base, ROM_OFFSET, length);
                } else {
                    memcpy(iov[i].iov_base, MIDI + *fd_offset, length);
                }
                *fd_offset += length;
                bytes_read += length;
            }

            return bytes_read;
        }
        case SYS_writev:
        {
            long (*write)(long, size_t) = NULL;
            if (a0 == FD_STDOUT || a0 == FD_STDERR) {
                write = ext_stdout;
            } else if (a0 == FD_MIDI) {
                write = write_midi;
            } else if (a0 == FD_DUMMY) {
                write = write_dummy;
            } else {
                fprintf(stderr, "WARN: write into an unknown FD: %li\n", a0);
                flush_stdio();

                return -EBADF;
            }

            const struct iovec *iov = (const struct iovec *)a1;
            long bytes_written = 0;
            for (long i = 0; i < a2; ++i) {
                long result = write(iov[i].iov_base, iov[i].iov_len);
                if (result < 0) {
                    return result;
                }
                bytes_written += result;
            }

            return bytes_written;
        }
        default:
        {
            fprintf(stderr, "WARN: unhandled syscall(3): %li\n", n);
            flush_stdio();

            return -ENOSYS;
        }
    }
}

long __syscall4(long n, long a0, long a1, long a2, long a3) {
    fprintf(stderr, "WARN: unhandled syscall(4): %li\n", n);
    flush_stdio();

    return -ENOSYS;
}

long __syscall6(long n, long a0, long a1, long a2, long a3, long a4, long a5) {
    fprintf(stderr, "WARN: unhandled syscall(6): %li\n", n);
    flush_stdio();

    return -ENOSYS;
}

#define AUDIO_SAMPLES_PER_CHUNK 512

static SDL_AudioDevice * AUDIO_DEVICE = NULL;
static int AUDIO_OpenDevice(SDL_AudioDevice *this, const char *devname) {
    this->spec.samples = AUDIO_SAMPLES_PER_CHUNK;
    this->spec.format = AUDIO_S16;
    this->spec.freq = 44100;
    this->spec.channels = 2;

    SDL_CalculateAudioSpec(&this->spec);

    printf("Opened audio device: buffer size = %lu\n", (unsigned long)this->spec.size);
    AUDIO_DEVICE = this;
    return 0;
}

static void AUDIO_CloseDevice(SDL_AudioDevice *this) {}

static SDL_bool AUDIO_Init(SDL_AudioDriverImpl *impl)
{
    impl->OpenDevice = AUDIO_OpenDevice;
    impl->CloseDevice = AUDIO_CloseDevice;
    impl->OnlyHasDefaultOutputDevice = SDL_TRUE;
    impl->ProvidesOwnCallbackThread = SDL_TRUE;
    return SDL_TRUE;
}

AudioBootStrap DUMMYAUDIO_bootstrap = {
    "doom", "DOOM audio", AUDIO_Init, SDL_FALSE
};

// This is based on SDL's Haiku backend.
void doom_get_audio(void * stream, size_t len)
{
    SDL_AudioDevice * audio = AUDIO_DEVICE;
    SDL_AudioCallback callback = audio->callbackspec.callback;
    SDL_LockMutex(audio->mixer_lock);

    if (!SDL_AtomicGet(&audio->enabled) || SDL_AtomicGet(&audio->paused)) {
        if (audio->stream) {
            SDL_AudioStreamClear(audio->stream);
        }
        SDL_memset(stream, audio->spec.silence, len);
    } else {
        SDL_assert(audio->spec.size == len);

        if (audio->stream == NULL) {  /* no conversion necessary. */
            callback(audio->callbackspec.userdata, (Uint8 *) stream, len);
        } else {  /* streaming/converting */
            const int stream_len = audio->callbackspec.size;
            const int ilen = (int) len;
            while (SDL_AudioStreamAvailable(audio->stream) < ilen) {
                callback(audio->callbackspec.userdata, audio->work_buffer, stream_len);
                if (SDL_AudioStreamPut(audio->stream, audio->work_buffer, stream_len) == -1) {
                    SDL_AudioStreamClear(audio->stream);
                    SDL_AtomicSet(&audio->enabled, 0);
                    break;
                }
            }

            const int got = SDL_AudioStreamGet(audio->stream, stream, ilen);
            SDL_assert((got < 0) || (got == ilen));
            if (got != ilen) {
                SDL_memset(stream, audio->spec.silence, len);
            }
        }
    }

    SDL_UnlockMutex(audio->mixer_lock);
}


#include "../libs/doomgeneric/doomgeneric/doomgeneric.h"

// EXPORTS

// NOTE: This *must* be static because Doom accesses these args not only during initialization.
static char * ARGV[3] = {"./doom", "-iwad", "doom1.wad"};

void ext_initialize() {
    doomgeneric_Create(3, ARGV);
}

void ext_tick(void) {
    doomgeneric_Tick();
}

static unsigned char _KEYS[256] = {};

#define KEYQUEUE_SIZE 32
static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

void ext_on_keychange(unsigned char key, unsigned char is_pressed) {
    unsigned char was_pressed = _KEYS[key];
    if (is_pressed == was_pressed) {
        return;
    }

    _KEYS[key] = is_pressed;

    unsigned short key_data = (is_pressed << 8) | key;
    s_KeyQueue[s_KeyQueueWriteIndex] = key_data;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

#ifdef __riscv

POLKAVM_EXPORT(void, ext_initialize);
POLKAVM_EXPORT(void, ext_tick);
POLKAVM_EXPORT(void, ext_on_keychange, uint8_t, uint8_t);

#endif

// ----------------------

void DG_Init(void) {}

void DG_DrawFrame(void) {
    ext_output_video(DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

static uint32_t SUBSAMPLES = 0;
static uint32_t SUBSAMPLES_EMITTED = 0;
static const uint32_t AUDIO_SUBSAMPLES_PER_TICK = 44100 / 100;
static const uint32_t SUBSAMPLES_PER_SAMPLE = 10;

#pragma GCC push_options
#pragma GCC optimize ("O3")

static void pump_audio() {
    for (;;) {
        uint32_t subsamples_pending = SUBSAMPLES - SUBSAMPLES_EMITTED;
        uint32_t samples_pending = subsamples_pending / SUBSAMPLES_PER_SAMPLE;
        if (samples_pending < AUDIO_SAMPLES_PER_CHUNK) {
            break;
        }

        char audio_buffer[AUDIO_SAMPLES_PER_CHUNK * 4];
        doom_get_audio(audio_buffer, AUDIO_SAMPLES_PER_CHUNK * 4);
        ext_output_audio(audio_buffer, AUDIO_SAMPLES_PER_CHUNK);
        SUBSAMPLES_EMITTED += SUBSAMPLES_PER_SAMPLE * AUDIO_SAMPLES_PER_CHUNK;
    }
}

void DG_SleepMs(uint32_t ms) {
    SUBSAMPLES += AUDIO_SUBSAMPLES_PER_TICK * ms;
    pump_audio();
}

uint32_t DG_GetTicksMs(void) {
    // Make sure there's always forward progress.
    SUBSAMPLES += 1;
    pump_audio();

    return SUBSAMPLES / AUDIO_SUBSAMPLES_PER_TICK;
}

void SDL_Delay(Uint32 ms) {
    DG_SleepMs(ms);
}

Uint32 SDL_GetTicks(void) {
    return SUBSAMPLES / AUDIO_SUBSAMPLES_PER_TICK;
}

#pragma GCC pop_options

int DG_GetKey(int * is_pressed, unsigned char * key) {
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
        return 0;
    }

    unsigned short key_data = s_KeyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *is_pressed = key_data >> 8;
    *key = key_data & 0xFF;

    return 1;
}

void DG_SetWindowTitle(const char * title) {}
