/*
MIT License

Copyright (c) 2023 Axel PASCON

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "arenalloc.h"

ARENA_BEGIN_DECLS

#include <errno.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(ARENA_ON_UNIX)
    #include <sys/mman.h>
    #include <unistd.h>
    #define ARENA_MORECORE(size) arena_morecore_unix(size)
#elif defined(ARENA_ON_MACOS)
    #include <sys/mman.h>
    #include <unistd.h>
    #define ARENA_MORECORE(size) arena_morecore_macos(size)
#elif defined(ARENA_ON_ANDROID)
    #include <sys/mman.h>
    #include <unistd.h>
    #define ARENA_MORECORE(size) arena_morecore_android(size)
#elif defined(ARENA_ON_WINDOWS)
    #include <windows.h>
    #define ARENA_MORECORE(size) arena_morecore_windows(size)
#else
    #error "Unsupported platform"
#endif

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
    #if defined(ARENA_MMAP_THRESHOLD)
        #undef ARENA_MMAP_THRESHOLD
    #endif
    #define ARENA_MMAP_THRESHOLD (128 * 1024)
#endif

static ARENA_LOCK_TYPE arena_raw_mem_morecore_lock;
static raw_mem_t static_reserved_memory[10] = {
    [0] = { .data = { 0 }, .state = { 0 } },
    [1] = { .data = { 0 }, .state = { 0 } },
    [2] = { .data = { 0 }, .state = { 0 } },
    [3] = { .data = { 0 }, .state = { 0 } },
    [4] = { .data = { 0 }, .state = { 0 } },
    [5] = { .data = { 0 }, .state = { 0 } },
    [6] = { .data = { 0 }, .state = { 0 } },
    [7] = { .data = { 0 }, .state = { 0 } },
    [8] = { .data = { 0 }, .state = { 0 } },
    [9] = { .data = { 0 }, .state = { 0 } }
};

static bool arena_is_initialized = false;

static void arena_align_ptr(void** ptr, size_t align);
static void* arena_raw_mem_morecore(size_t size);

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)

static void* initial_data_end = NULL;

static ARENA_LOCK_TYPE arena_mmap_threshold_lock;
static size_t arena_mmap_threshold = ARENA_MMAP_THRESHOLD;

void arena_set_mmap_threshold(size_t size)
{
    ARENA_LOCK(arena_mmap_threshold_lock);
    arena_mmap_threshold = size;
    ARENA_UNLOCK(arena_mmap_threshold_lock);
}

#if defined(ARENA_ON_UNIX)

static void* arena_morecore_unix(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            return NULL;
            
        return ptr;
    }
    else
    {
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#elif defined(ARENA_ON_MACOS)

static void* arena_morecore_macos(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            return NULL;
            
        return ptr;
    }
    else
    {
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#else

static void* arena_morecore_android(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            return NULL;
            
        return ptr;
    }
    else
    {
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#endif

#else

static void* arena_morecore_windows(size_t size)
{

}

#endif

void arenalloc_init()
{
    if (arena_is_initialized)
        return;

    #if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
        initial_data_end = sbrk(0);
        ARENA_LOCK_INIT(arena_mmap_threshold_lock);
    #endif
    ARENA_LOCK_INIT(arena_raw_mem_morecore_lock);

    atexit(arenalloc_deinit);
    #if (defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011)
    at_quick_exit(arenalloc_deinit);
    #endif
    arena_is_initialized = true;
}

void arenalloc_deinit()
{
    if (!arena_is_initialized)
        return;

    bool need_exit = false;
    errno = 0;
    #if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
        if (brk(initial_data_end) == -1 && initial_data_end != sbrk(0))
        {
            fprintf(stderr, "Failed to deinitialize arena: %s\n", strerror(errno));
            need_exit = true;
        }
        ARENA_LOCK_DESTROY(arena_mmap_threshold_lock);
    #endif
    ARENA_LOCK_DESTROY(arena_raw_mem_morecore_lock);

    arena_is_initialized = false;
    need_exit ? exit(EXIT_FAILURE) : (void)0;
}

static void arena_align_ptr(void** ptr, size_t align)
{
    uintptr_t addr = (uintptr_t) *ptr;
    uintptr_t aligned_addr = (addr + align - 1) & -align;
    *ptr = (void*) aligned_addr;
}

// Search for `size` bytes of memory in the static reserved memory
static void* arena_raw_mem_morecore(size_t size)
{
    ARENA_LOCK(arena_raw_mem_morecore_lock);
    static size_t start_from[2] = { 0, 0 }; // i, j, in the loops below
    size_t total_free = 0;
    size_t free_count = 0;
    for (size_t i = start_from[0]; i < 10; ++i)
    {
        for (size_t j = start_from[1]; j < ARENA_STATIC_CAP / 10; ++j)
        {
            // TODO: Finish this
            if (static_reserved_memory[i].state[j] == 0)
            {
                ++free_count;
                ++total_free;
                if (free_count == size + 1)
                {
                    void* ptr = static_reserved_memory[i].data + j - size;
                    memset(ptr, 0, size);
                    start_from[0] = i;
                    start_from[1] = j;
                    ARENA_UNLOCK(arena_raw_mem_morecore_lock);
                    return ptr;
                }
            }
            else
            {
                free_count = 0;
            }
        }
    }

    ARENA_UNLOCK(arena_raw_mem_morecore_lock);
    return NULL;
}
ARENA_END_DECLS