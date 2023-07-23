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

#if defined(INIT_BITMAP)
    #undef INIT_BITMAP
#endif
#define INIT_BITMAP (bit_map_t){ 0 }

#if defined(INIT_STATIC_RAW_MEM_T)
    #undef INIT_STATIC_RAW_MEM_T
#endif
#define INIT_STATIC_RAW_MEM_T(arr_index)                                            \
    [arr_index] = {                                                                 \
        .data = &(static_bytes[(ARENA_STATIC_CAP / 10) * arr_index]),               \
        .state = INIT_BITMAP,                                                       \
        .original_pointer = &(static_bytes[(ARENA_STATIC_CAP / 10) * arr_index]),   \
        .kind = ARENA_STATIC_MEM                                                    \
    }

// TODO: Make all source-file-scope function declarations static only if ARENA_DEBUG is not defined

/*
 * Memory related stuff
 * {
 */

static char static_bytes[ARENA_STATIC_CAP] = { 0 };
static raw_mem_t static_reserved_memory[10] = {
    INIT_STATIC_RAW_MEM_T(0),
    INIT_STATIC_RAW_MEM_T(1),
    INIT_STATIC_RAW_MEM_T(2),
    INIT_STATIC_RAW_MEM_T(3),
    INIT_STATIC_RAW_MEM_T(4),
    INIT_STATIC_RAW_MEM_T(5),
    INIT_STATIC_RAW_MEM_T(6),
    INIT_STATIC_RAW_MEM_T(7),
    INIT_STATIC_RAW_MEM_T(8),
    INIT_STATIC_RAW_MEM_T(9)
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
#if 0
typedef struct ArenaPointer
{
    void* ptr;
    struct ArenaPointer* next;
    size_t size;
} arena_ptr_t;
// Keep track of all pointers internally used by arenalloc that must be freed at the end of the program
static arena_ptr_t* arena_ptr_list = NULL;
static ARENA_LOCK_TYPE arena_ptr_list_lock;
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
static uintptr_t arena_align_ptr(void** ptr, size_t align);
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

#if 0
#if !defined(ARENA_DEBUG)
#if defined(arena_static_mem_alloc)
    #undef arena_static_mem_alloc
#endif
#define arena_static_mem_alloc(...)                                                                             \
    ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 0))                                                       \
    (ARENA_STATIC_ASSERT(false, "arena_static_mem_alloc() must be called with at least one argument"), NULL)    \
    (ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 1))                                                      \
        (arena_static_mem_alloc_default(__VA_ARGS__))                                                           \
        (arena_static_mem_alloc_aligned(__VA_ARGS__)))

static void* arena_static_mem_alloc_default(size_t size);
static void* arena_static_mem_alloc_aligned(size_t size, size_t align);
#endif
#endif

    /*
     * } // Global utility functions
     */

    /*
     * Raw memory related stuff
     * {
     */

        /*
         * Bitmap related stuff
         * {
         */

#if !defined(ARENA_DEBUG)
static size_t bitmap_first_fit(size_t size, size_t align, const char* const data, bit_map_t bitmap);
static bool bitmap_set(size_t start, size_t size, bit_map_t bitmap, bool value);
#endif
        /*
         * } // Bitmap related stuff
         */
#if 0
#if !defined(ARENA_DEBUG)
static void* arena_static_mem_alloc_default(size_t size);
static void* arena_static_mem_alloc_aligned(size_t size, size_t align);
#endif
#endif
    /*
     * } // Raw memory related stuff
     */

    /*
     * Arena related stuff
     * {
     */

static inline void arena_add_to_list(arena_t* arena, arena_t** list);
static inline void arena_remove_from_list(arena_t* arena, arena_t** list);

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

static
long arena_win_page_size(void)
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

void
arenalloc_init(void)
{
    if (arena_is_initialized)
        return;

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
        ARENA_LOCK_INIT(arena_mmap_threshold_lock);
#endif

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

    arena_is_initialized = false;
    need_exit ? _Exit(EXIT_FAILURE) : (void)0;
}

static inline bool
is_power_of(size_t x, size_t base)
{
    return x == 0 ? false : (x == 1 ? true : (x % base == 0 ? is_power_of(x / base, base) : false));
}

static uintptr_t
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

#if 0

#if !defined(ARENA_DEBUG)
static
#endif
// Search for `size` bytes of memory in the static reserved memory
void* arena_static_mem_alloc_default(size_t size)
{
    return arena_static_mem_alloc_aligned(size, ARENA_DEFAULT_ALIGN);
}

ARENA_ALLOCATOR((), (1), 2)
#if !defined(ARENA_DEBUG)
static
#endif
void* arena_static_mem_alloc_aligned(size_t size, size_t align)
{
}
#endif

ARENA_STATIC_ASSERT(ARENA_STATIC_MEM_BASE_ALIGN >= ARENA_STATIC_MEM_MAX_ALIGN_REQUEST, "ARENA_STATIC_MEM_BASE_ALIGN must be greater than or equal to ARENA_STATIC_MEM_MAX_ALIGN_REQUEST");
#if !defined(ARENA_DEBUG)
static
#endif
/*
 * Each bit in the bitmap represents a byte in the `static` raw memory.
 * If the bit is set, the byte is used, otherwise it is free. This function
 * searches for `size` consecutive bytes of free memory in the `static` raw.
 */
size_t
bitmap_first_fit(size_t size, size_t align, const char* const data, bit_map_t bitmap)
{
    static size_t start_from = 0;

#if 0
    bool needed_to_lock = false;
    if (ARENA_LOCK_IS_LOCKED(arena_static_mem_lock) && ARENA_LOCK_IS_MINE(arena_static_mem_lock)) goto skip_lock;
    else
    {
        ARENA_LOCK(arena_static_mem_lock);
        needed_to_lock = true;
    }
#endif

skip_lock:
    size_t ret_val = (size_t) (-1);
    size_t available = 0;
    size_t count = 0;
    const size_t i_start = start_from / 8;
    const size_t i_max = ARENA_BITMAP_SIZE;
    /* Iterates over the bitmap bytes: each `i` represents a byte in the bitmap */
    for (size_t i = i_start; count < ARENA_BITMAP_SIZE; ++i)
    {
        uint8_t popcount = ARENA_POPCOUNT(bitmap[i], 8);
        if (popcount == 0)
        {
            available += 8;
            if (available >= size)
            {
                // Consider alignment
                size_t prev_available = available;
                bool break_and_continue = false;
                bool positive_index = ((i + 1) * 8) >= available;
                ARENA_ASSERT(positive_index);
                if (!positive_index)
                {
                    ret_val = (size_t) (-1);
                    goto ret_point;
                }
                while ((uintptr_t) &(data[(((i + 1) * 8) - available)]) % (uintptr_t) align != 0)
                {
                    if (available == 0)
                    {
                        break_and_continue = true;
                        available = prev_available;
                        break;
                    }
                    --available;
                    if (available < size)
                    {
                        break_and_continue = true;
                        available = prev_available;
                        break;
                    }
                    positive_index = (((i + 1) * 8)) >= available;
                    ARENA_ASSERT(positive_index);
                    if (!positive_index)
                    {
                        ret_val = (size_t) (-1);
                        goto ret_point;
                    }
                }

                if (break_and_continue)
                {
                    available = prev_available;
                    continue;
                }

                bool aligned = ((uintptr_t) &(data[(((i + 1) * 8) - available)]) % (uintptr_t) align == 0);
                positive_index = (((i + 1) * 8) >= available);
                ARENA_ASSERT(aligned && positive_index);
                if (!aligned || !positive_index)
                {
                    ret_val = (size_t) (-1);
                    goto ret_point;
                }

                if (!bitmap_set(((i + 1) * 8) - available, size, bitmap, true))
                {
                    ret_val = (size_t) (-1);
                    goto ret_point;
                }

                ret_val = ((i + 1) * 8) - available;
                goto ret_point;
            }
            else
                continue;
        }
        else if (popcount == 8)
        {
            available = 0;
            continue;
        }
        else
        {
            /* Iterates over the bits in the byte: each `j` represents a bit in the byte */
            for (size_t j = 0; j < 8; ++j)
            {
                /* We start from the most significant bit, i.e. the leftmost bit */
                if (ARENA_BITMAP_BIT_INDEX(bitmap[i], j) == 0)
                {
                    ++available;
                    if (available >= size)
                    {
                        size_t prev_available = available;
                        bool break_and_continue = false;
                        bool positive_index = ((i * 8) + j + 1) >= available;
                        ARENA_ASSERT(positive_index);
                        if (!positive_index)
                        {
                            ret_val = (size_t) (-1);
                            goto ret_point;
                        }
                        while ((uintptr_t) &(data[((i * 8) + j + 1) - available]) % (uintptr_t) align != 0)
                        {
                            if (available == 0)
                            {
                                break_and_continue = true;
                                available = prev_available;
                                break;
                            }
                            --available;
                            if (available < size)
                            {
                                break_and_continue = true;
                                available = prev_available;
                                break;
                            }
                            positive_index = (((i * 8) + j + 1) >= available);
                            ARENA_ASSERT(positive_index);
                            if (!positive_index)
                            {
                                ret_val = (size_t) (-1);
                                goto ret_point;
                            }
                        }

                        if (break_and_continue)
                        {
                            available = prev_available;
                            continue;
                        }

                        bool aligned = ((uintptr_t) &(data[((i * 8) + j + 1) - available]) % (uintptr_t) align == 0);
                        positive_index = (((i * 8) + j + 1) >= available);
                        ARENA_ASSERT(aligned && positive_index);
                        if (!aligned || !positive_index)
                        {
                            ret_val = (size_t) (-1);
                            goto ret_point;
                        }
                        
                        if (!bitmap_set((i * 8) + j + 1 - available, size, bitmap, true))
                        {
                            ret_val = (size_t) (-1);
                            goto ret_point;
                        }

                        ret_val = (i * 8) + j + 1 - available;
                        goto ret_point;
                    }
                }
                else
                {
                    available = 0;
                }
            }
        }

        if (i == i_max - 1)
        {
            i = (size_t) (-1); // Will wrap around to 0
        }
        ++count;
    }

ret_point:
#if 0
    if(needed_to_lock)
    {
        ARENA_UNLOCK(arena_static_mem_lock);
    }
#endif
    start_from = ret_val;
    return ret_val;
}

#if !defined(ARENA_DEBUG)
static
#endif
bool
bitmap_set(size_t start, size_t size, bit_map_t bitmap, bool value)
{
    if (start + size > ARENA_BITMAP_SIZE * 8)
        return false;

    if (value)
    {
        // Assert previously free
        for (size_t i = start; i < start + size; ++i)
        {
            bool cond = (bitmap[i / 8] & (1 << (7 - (i % 8)))) == 0;
            ARENA_ASSERT(cond);
            // In case of NDEBUG, we return false if the assertion fails
            if (!cond)
                return false;
        }
    }
    else
    {
        // Assert previously used
        for (size_t i = start; i < start + size; ++i)
        {
            bool cond = (bitmap[i / 8] & (1 << (7 - (i % 8)))) != 0;
            ARENA_ASSERT(cond);
            // In case of NDEBUG, we return false if the assertion fails, too
            if (!cond)
                return false;
        }
    }

    for (size_t i = start; i < start + size; ++i)
    {
        if (value)
            bitmap[i / 8] |= (1 << (7 - (i % 8)));
        else
            bitmap[i / 8] &= ~(1 << (7 - (i % 8)));
    }

    return true;
}

#if defined(ARENA_DEBUG)
void
bitmap_print(FILE* stream, size_t start, size_t size, bit_map_t bitmap)
{
    for (size_t i = start; i < start + size; ++i)
    {
        fprintf(stream, ((i % 8 == 0) && (i != 0) ? " " : ""));
        fprintf(stream, "%" PRIu8, (bitmap[i / 8] & (1 << (7 - (i % 8)))) != 0);
    }
    fprintf(stream, "\n");
}
#endif

#if 0
static raw_mem_t*
arena_raw_mem_create(size_t size)
{
    if (!size)
        return NULL;

    raw_mem_t* raw_mem = ARENA_MORECORE(sizeof(raw_mem_t) + ARENA_ALIGNOF(raw_mem_t));
    if (!raw_mem)
        return NULL;
    raw_mem->kind = ARENA_DYN_MEM;
    size_t full_size = ARENA_MORECORE_ALIGNED(size, ARENA_DEFAULT_ALIGN, &(raw_mem->data));
}
#endif

/* Remove from circular doubly linked list */
static inline void
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

/* Add to circular doubly linked list */

static inline void
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
    if (size == 0)
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

    char* ptr = ARENA_MORECORE(size + ARENA_ALIGNOF(raw_mem_t) + (size + ARENA_ALIGNOF(raw_mem_t)) % MEMORY_PAGE_SIZE);
    if (!ptr)
        return NULL;
    char* mem_space = ptr;
    (void) arena_align_ptr((void**) &mem_space, ARENA_ALIGNOF(raw_mem_t));
    ARENA_ASSERT((uintptr_t) mem_space % ARENA_ALIGNOF(raw_mem_t) == 0);
    raw_mem_t* raw_mem = ARENA_MORECORE(sizeof(raw_mem_t));
    if (!raw_mem)
        return NULL;
    raw_mem->kind = ARENA_DYN_MEM;
    raw_mem->data = mem_space;
    (void) memset(
        raw_mem->data,
        0,
        size + ARENA_ALIGNOF(raw_mem_t) + (size + ARENA_ALIGNOF(raw_mem_t)) % MEMORY_PAGE_SIZE
            - ((uintptr_t) raw_mem->data - (uintptr_t) ptr)
    );
    raw_mem->original_pointer = ptr;
}
