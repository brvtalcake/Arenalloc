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

/*
 * How to use arenalloc:
 * 
 * 1. Include arenalloc.h in your source file
 * 2. Call arenalloc_init() at the beginning of your program
 * 3. (optional, automatic) Call arenalloc_deinit() at the end of your program
 * 4. To allocate memory with arenalloc, you need to follow these steps:
 *     1. Call arena_create() with the size of the memory you want your arena to have
 *     2. Call arena_alloc() with the size of the memory you want to allocate, the alignment you want,
 *        and the arena you want to allocate from
 *     3. Use the memory returned by arena_alloc() as you wish
 *     4. Call arena_free() with the memory you want to free and the arena you want to free from
 *     5. Call arena_destroy() with the arena you want to destroy
 * 
 * Internally, arenalloc uses a raw_mem_t structure to represent the memory an arena has. For little programs,
 * and in order to avoid calling sbrk() and friends too often, arenalloc also uses static raw_mem_t's. As soon
 * as the static raw_mem_t's are full, arenalloc will call sbrk() to get more memory.
 */

#include "arenalloc.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
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
    #define MEMORY_PAGE_SIZE sysconf(_SC_PAGESIZE)
#elif defined(ARENA_ON_MACOS)
    #include <sys/mman.h>
    #include <unistd.h>
    #define ARENA_MORECORE(size) arena_morecore_macos(size)
    #define MEMORY_PAGE_SIZE sysconf(_SC_PAGESIZE)
#elif defined(ARENA_ON_ANDROID)
    #include <sys/mman.h>
    #include <unistd.h>
    #define ARENA_MORECORE(size) arena_morecore_android(size)
    #define MEMORY_PAGE_SIZE sysconf(_SC_PAGESIZE)
#elif defined(ARENA_ON_WINDOWS)
    #include <windows.h>
    #define ARENA_MORECORE(size) arena_morecore_windows(size)
    #define MEMORY_PAGE_SIZE arena_win_page_size()
#else
    #error "Unsupported platform"
#endif

#undef ARENA_FACTOR
#define ARENA_FACTOR (2 * 2 * 2)
#if !defined(ARENA_DEFAULT_CAP)
/* Default apacity of each non-static arenas */
    #define ARENA_DEFAULT_CAP (MEMORY_PAGE_SIZE * ARENA_FACTOR)
#endif
#if !defined(ARENA_STATIC_COUNT)
/* Number of static arenas */
    #define ARENA_STATIC_COUNT 10
#endif
#if !defined(ARENA_STATIC_DEFAULT_CAP)
/* Default apacity of each static arenas */
    #define ARENA_STATIC_DEFAULT_CAP (4096 * ARENA_FACTOR)
#endif

#if defined(ARENA_POPCOUNT)
    #undef ARENA_POPCOUNT
#endif
#define ARENA_POPCOUNT(x, size) ARENA_CAT(ARENA_POPCOUNT_, size)(x)

#if defined(ARENA_POPCOUNT_8)
    #undef ARENA_POPCOUNT_8
#endif
#if defined(ARENA_WITH_MSVC)
    #include <intrin.h>
    #define ARENA_POPCOUNT_8(x) ((uint8_t) __popcnt16((uint16_t) 0xFF & (uint16_t) x))
#else
    #define ARENA_POPCOUNT_8(x) ((uint8_t) __builtin_popcount((int) 0xFF & (int) x))
#endif

#if defined(ARENA_POPCOUNT_16)
    #undef ARENA_POPCOUNT_16
#endif
#if defined(ARENA_WITH_MSVC)
    #include <intrin.h>
    #define ARENA_POPCOUNT_16(x) ((uint16_t) __popcnt16((uint16_t) 0xFFFF & (uint16_t) x))
#else
    #define ARENA_POPCOUNT_16(x) ((uint16_t) __builtin_popcount((int) 0xFFFF & (int) x))
#endif

#if defined(ARENA_POPCOUNT_32)
    #undef ARENA_POPCOUNT_32
#endif
#if defined(ARENA_WITH_MSVC)
    #include <intrin.h>
    #define ARENA_POPCOUNT_32(x) ((uint32_t) __popcnt((uint32_t) 0xFFFFFFFF & (uint32_t) x))
#else
    #define ARENA_POPCOUNT_32(x) ((uint32_t) __builtin_popcountl((long) 0xFFFFFFFF & (long) x))
#endif

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
    #if defined(ARENA_MMAP_THRESHOLD)
        #undef ARENA_MMAP_THRESHOLD
    #endif
    #define ARENA_MMAP_THRESHOLD (128 * 1024)
#endif

#if defined(ARENA_BITMAP_BIT_INDEX)
    #undef ARENA_BITMAP_BIT_INDEX
#endif
// TODO: Can we just do `((u8_byte) & (1 << i))`?
#define ARENA_BITMAP_BIT_INDEX(u8_byte, i) ((((u8_byte) >> (7 - (i))) & UINT8_C(0x1)) != 0)

#if defined(INIT_STATIC_ARENA_T)
    #undef INIT_STATIC_ARENA_T
#endif
#define INIT_STATIC_ARENA_T(arr_index)      \
    [arr_index] = {                         \
        .lock   = { 0 },                    \
        .next   = NULL,                     \
        .prev   = NULL,                     \
        .first  = NULL,                     \
        .size   = ARENA_STATIC_DEFAULT_CAP, \
    }

// TODO: Make all source-file-scope function declarations static only if ARENA_DEBUG is not defined

/*
 * Memory related stuff
 * {
 */

ARENA_ALIGNAS(ARENA_STATIC_DEFAULT_CAP / ARENA_FACTOR)
static char static_bytes[ARENA_STATIC_DEFAULT_CAP * ARENA_STATIC_COUNT] = { 0 };
static arena_t static_arenas[ARENA_STATIC_COUNT] = {
    INIT_STATIC_ARENA_T(0),
    INIT_STATIC_ARENA_T(1),
    INIT_STATIC_ARENA_T(2),
    INIT_STATIC_ARENA_T(3),
    INIT_STATIC_ARENA_T(4),
    INIT_STATIC_ARENA_T(5),
    INIT_STATIC_ARENA_T(6),
    INIT_STATIC_ARENA_T(7),
    INIT_STATIC_ARENA_T(8),
    INIT_STATIC_ARENA_T(9),
};
static arena_t* used_arena_list = NULL;
static arena_t* free_arena_list = NULL;
static ARENA_LOCK_TYPE ua_list_lock;
static ARENA_LOCK_TYPE fa_list_lock;

/*
 * } // Memory related stuff
 */

/*
 * Library state related stuff
 * {
 */

static bool arena_is_initialized = false;
static bool arena_is_registered_at_exit = false;
#if (defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011)
static bool arena_is_registered_at_quick_exit = false;
#endif
#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
static ARENA_LOCK_TYPE arena_mmap_threshold_lock;
static size_t arena_mmap_threshold = ARENA_MMAP_THRESHOLD;
#endif
/*
 * } // Library state related stuff
 */

/* 
 * Functions' forward declarations
 * {
 */

    /*
     * Global utility functions
     * {
     */
static inline bool is_power_of(size_t x, size_t base);
static inline uintptr_t arena_align_ptr(void** ptr, size_t align);
#if defined(ARENA_ON_UNIX)
static void* arena_morecore_unix(size_t size);
#elif defined(ARENA_ON_MACOS)
static void* arena_morecore_macos(size_t size);
#elif defined(ARENA_ON_ANDROID)
static void* arena_morecore_android(size_t size);
#elif defined(ARENA_ON_WINDOWS)
static long arena_win_page_size(void);
static void* arena_morecore_windows(size_t size);
#else
    #error "Unsupported platform"
#endif

    /*
     * } // Global utility functions
     */

    /*
     * Arena related stuff
     * {
     */

#if !defined(ARENA_DEBUG)
static inline void arena_add_to_list(arena_t* arena, arena_t** list);
static inline void arena_remove_from_list(arena_t* arena, arena_t** list);
#endif
    /*
     * } // Arena related stuff
     */

/*
 * } // Functions' forward declarations
 */

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)

void
arena_set_mmap_threshold(size_t size)
{
    ARENA_LOCK(arena_mmap_threshold_lock);
    arena_mmap_threshold = size;
    ARENA_UNLOCK(arena_mmap_threshold_lock);
}

#if defined(ARENA_ON_UNIX)

static void*
arena_morecore_unix(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            goto mmap_fallback;
            
        return ptr;
    }
    else
    {
mmap_fallback:
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#elif defined(ARENA_ON_MACOS)

static void*
arena_morecore_macos(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            goto mmap_fallback;
            
        return ptr;
    }
    else
    {
mmap_fallback:
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#else

static void*
arena_morecore_android(size_t size)
{
    if (size < arena_mmap_threshold)
    {
        void* ptr = sbrk(0);
        void* request = sbrk(size);

        if (request == (void*) -1)
            goto mmap_fallback;
            
        return ptr;
    }
    else
    {
mmap_fallback:
        void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
            return NULL;

        return ptr;
    }
}

#endif

#elif defined(ARENA_ON_WINDOWS)

static long
arena_win_page_size(void)
{
    static long page_size = 0;
    if (page_size == 0)
    {
        SYSTEM_INFO system_info;
        GetSystemInfo(&system_info);
        page_size = system_info.dwPageSize;
    }
    return page_size;
}

static void*
arena_morecore_windows(size_t size)
{
    void* ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return ptr;
}

#else
    #error "Unsupported platform"
#endif

ARENA_FUNC_CONSTRUCTOR
void
arenalloc_init(void)
{
    if (arena_is_initialized)
        return;

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
    ARENA_LOCK_INIT(arena_mmap_threshold_lock);
#endif
    ARENA_LOCK_INIT(ua_list_lock);
    ARENA_LOCK_INIT(fa_list_lock);

    for (size_t i = 0; i < ARENA_STATIC_COUNT; ++i)
    {
        uintptr_t start = (uintptr_t)(static_bytes + (i * ARENA_STATIC_DEFAULT_CAP) + sizeof(mem_header_t));
        start += start % ARENA_DEFAULT_ALIGN;
        uintptr_t end = (uintptr_t)(static_bytes + ((i + 1) * ARENA_STATIC_DEFAULT_CAP));

        ARENA_ASSERT(start % ARENA_DEFAULT_ALIGN == 0);
        ARENA_ASSERT((uintptr_t)(static_bytes + (i * ARENA_STATIC_DEFAULT_CAP)) % ARENA_ALIGNOF(mem_header_t) == 0);
        ARENA_ASSERT(start - (uintptr_t)(static_bytes + (i * ARENA_STATIC_DEFAULT_CAP)) >= sizeof(mem_header_t));

        mem_header_t header = {
            .next = NULL,
            .start = (char*) start,
            .end = (char*) end,
            .requested_align = ARENA_DEFAULT_ALIGN,
            .in_use = false
        };
        memcpy(static_bytes + (i * ARENA_STATIC_DEFAULT_CAP), &header, sizeof(mem_header_t));
        arena_t* arena = static_arenas + i;
        arena->first = (mem_header_t*) (static_bytes + (i * ARENA_STATIC_DEFAULT_CAP));
        arena->size = ARENA_STATIC_DEFAULT_CAP;
        arena_add_to_list(arena, &free_arena_list);
    }

    if (!arena_is_registered_at_exit)
    {
        atexit(arenalloc_deinit);
        arena_is_registered_at_exit = true;
    }
#if (defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011)
    if (!arena_is_registered_at_quick_exit)
    {
        at_quick_exit(arenalloc_deinit);
        arena_is_registered_at_quick_exit = true;
    }
#endif
    arena_is_initialized = true;
}

ARENA_FUNC_DESTRUCTOR
void
arenalloc_deinit(void)
{
    if (!arena_is_initialized)
        return;

    bool need_exit = false;
    errno = 0;
    
#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
    ARENA_LOCK_DESTROY(arena_mmap_threshold_lock);
#endif
    ARENA_LOCK_DESTROY(ua_list_lock);
    ARENA_LOCK_DESTROY(fa_list_lock);

    arena_is_initialized = false;
    need_exit ? _Exit(EXIT_FAILURE) : (void)0;
}

static inline bool
is_power_of(size_t x, size_t base)
{
    return x == 0 ? false : (x == 1 ? true : (x % base == 0 ? is_power_of(x / base, base) : false));
}

static inline uintptr_t
arena_align_ptr(void** ptr, size_t align)
{
    uintptr_t offset = 0;
    uintptr_t addr = (uintptr_t) *ptr;
    uintptr_t aligned_addr = addr;

    if (is_power_of(align, 2)) goto power_of_two;

    while (aligned_addr % align != 0)
        ++aligned_addr;
    *ptr = (void*) aligned_addr;
    offset = aligned_addr - addr;
    return offset;

power_of_two:
    aligned_addr = (addr + align - 1) & ~(align - 1);
    *ptr = (void*) aligned_addr;
    offset = aligned_addr - addr;
    return offset;
}

#if !defined(ARENA_DEBUG)
static
#endif
/* Remove from circular doubly linked list */
inline void
arena_remove_from_list(arena_t* arena, arena_t** list)
{
    if (!arena || !list)
        return;
    
    if (arena->next == arena || (arena->prev == arena && arena->next == arena))
    {
        *list = NULL;
        arena->next = NULL;
        arena->prev = NULL;
        return;
    }

    arena->prev->next = arena->next;
    arena->next->prev = arena->prev;
    if (*list == arena)
        *list = arena->next;
    arena->next = NULL;
    arena->prev = NULL;
}

#if !defined(ARENA_DEBUG)
static
#endif
/* Add to circular doubly linked list */
inline void
arena_add_to_list(arena_t* arena, arena_t** list)
{
    if (!arena || !list)
        return;

    if (!*list)
    {
        *list = arena;
        arena->next = arena;
        arena->prev = arena;
        return;
    }

    arena->next = *list;
    arena->prev = (*list)->prev;
    (*list)->prev->next = arena;
    (*list)->prev = arena;
    *list = arena;
}

/* This function is used by users to get a brand new arena */
arena_t*
arena_create(size_t size)
{
    if (size == 0 || !arena_is_initialized)
        return NULL;

    ARENA_LOCK(fa_list_lock);
    ARENA_LOCK(ua_list_lock);
    bool looped = false;
    bool started = true;
    for (arena_t* arena = free_arena_list; looped; arena = arena->next)
    {
        if (arena->size >= size)
        {
            arena_remove_from_list(arena, &free_arena_list);
            arena_add_to_list(arena, &used_arena_list);
            ARENA_UNLOCK(ua_list_lock);
            ARENA_UNLOCK(fa_list_lock);
            return arena;
        }
        if (arena == free_arena_list && !started)
            looped = true;
        started = false;
    }
    ARENA_UNLOCK(ua_list_lock);
    ARENA_UNLOCK(fa_list_lock);
}
