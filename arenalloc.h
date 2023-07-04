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

#ifndef ARENALLOC_H
#define ARENALLOC_H

#if defined(__INTELLISENSE__)
    #define ARENA_DEBUG 1
#endif

#include <stddef.h>

#if defined(ARENA_NAMESPACE)
    #undef ARENA_NAMESPACE
#endif
#if defined(ARENA_CAT)
    #undef ARENA_CAT
#endif
#if defined(ARENA_CAT_REDIRECT)
    #undef ARENA_CAT_REDIRECT
#endif
#if defined(ARENA_C)
    #undef ARENA_C
#endif
#if defined(ARENA_CXX)
    #undef ARENA_CXX
#endif
#if defined(ARENA_HAS_FEATURE)
    #undef ARENA_HAS_FEATURE
#endif
#if defined(ARENA_HAS_EXTENSION)
    #undef ARENA_HAS_EXTENSION
#endif
#if defined(ARENA_HAS_INCLUDE)
    #undef ARENA_HAS_INCLUDE
#endif
#if defined(ARENA_WITH_GCC)
    #undef ARENA_WITH_GCC
#endif
#if defined(ARENA_WITH_CLANG)
    #undef ARENA_WITH_CLANG
#endif
#if defined(ARENA_WITH_MSVC)
    #undef ARENA_WITH_MSVC
#endif
#if defined(ARENA_ON_WINDOWS)
    #undef ARENA_ON_WINDOWS
#endif
#if defined(ARENA_ON_UNIX)
    #undef ARENA_ON_UNIX
#endif
#if defined(ARENA_ON_MACOS)
    #undef ARENA_ON_MACOS
#endif
#if defined(ARENA_ON_ANDROID)
    #undef ARENA_ON_ANDROID
#endif
#if defined(ARENA_HAS_THREADS)
    #undef ARENA_HAS_THREADS
#endif
#if defined(ARENA_HAS_ATOMIC)
    #undef ARENA_HAS_ATOMIC
#endif
#if defined(ARENA_ATOMIC_SPECIFIED_TYPE)
    #undef ARENA_ATOMIC_SPECIFIED_TYPE
#endif
#if defined(ARENA_ATOMIC_QUALIFIED_TYPE)
    #undef ARENA_ATOMIC_QUALIFIED_TYPE
#endif
#if defined(ARENA_LOCK_TYPE)
    #undef ARENA_LOCK_TYPE
#endif
#if defined(ARENA_LOCK_INIT)
    #undef ARENA_LOCK_INIT
#endif
#if defined(ARENA_LOCK_DESTROY)
    #undef ARENA_LOCK_DESTROY
#endif
#if defined(ARENA_LOCK)
    #undef ARENA_LOCK
#endif
#if defined(ARENA_UNLOCK)
    #undef ARENA_UNLOCK
#endif
#if defined(ARENA_THREAD_SAFE)
    #undef ARENA_THREAD_SAFE
#endif
#if defined(ARENA_LOCK_INT_TYPE)
    #undef ARENA_LOCK_INT_TYPE
#endif
#if defined(ARENA_MK_LOCK_INT_TYPE)
    #undef ARENA_MK_LOCK_INT_TYPE
#endif
#if defined(ARENA_MORECORE)
    #undef ARENA_MORECORE
#endif
#if defined(ARENA_BEGIN_DECLS)
    #undef ARENA_BEGIN_DECLS
#endif
#if defined(ARENA_END_DECLS)
    #undef ARENA_END_DECLS
#endif
#if defined(ARENA_ALIGNAS)
    #undef ARENA_ALIGNAS
#endif
#if defined(ARENA_ALIGNOF)
    #undef ARENA_ALIGNOF
#endif
#if defined(ARENA_BITMAP_SIZE)
    #undef ARENA_BITMAP_SIZE
#endif

#define ARENA_CAT_REDIRECT(x, y) x##y
#define ARENA_CAT(x, y) ARENA_CAT_REDIRECT(x, y)
#define ARENA_NAMESPACE(ident) ARENA_CAT(arena_, ident)

#if defined(__has_feature)
    #define ARENA_HAS_FEATURE(x) __has_feature(x)
#else
    #define ARENA_HAS_FEATURE(x) 0
#endif

#if defined(__has_extension)
    #define ARENA_HAS_EXTENSION(x) __has_extension(x)
#else
    #define ARENA_HAS_EXTENSION(x) 0
#endif

#if defined(__has_include)
    #define ARENA_HAS_INCLUDE(x) __has_include(x)
#else
    #define ARENA_HAS_INCLUDE(x) 0
#endif

#if defined(__GNUC__)
    #define ARENA_WITH_GCC 1
#elif defined(__clang__)
    #define ARENA_WITH_CLANG 1
#elif defined(_MSC_VER)
    #define ARENA_WITH_MSVC 1
#else
    #warning "Unknown compiler"
#endif

#if defined(__cplusplus)
    #if defined(ARENA_WITH_MSVC)
        #if defined(_MSVC_LANG)
            #if _MSVC_LANG <= 199711L
                #define ARENA_CXX 1998
            #elif _MSVC_LANG <= 201103L
                #define ARENA_CXX 2011
            #elif _MSVC_LANG <= 201402L
                #define ARENA_CXX 2014
            #elif _MSVC_LANG <= 201703L
                #define ARENA_CXX 2017
            #else
                #define ARENA_CXX 2023
            #endif
        #else
            #define ARENA_CXX 1998
        #endif
    #else
        #if __cplusplus <= 199711L
            #define ARENA_CXX 1998
        #elif __cplusplus <= 201103L
            #define ARENA_CXX 2011
        #elif __cplusplus <= 201402L
            #define ARENA_CXX 2014
        #elif __cplusplus <= 201703L
            #define ARENA_CXX 2017
        #else
            #define ARENA_CXX 2023
        #endif
    #endif
#elif defined(__STDC__)
    #if defined(ARENA_WITH_MSVC)
        #define ARENA_C 1999
    #else
        #if !defined(__STDC_VERSION__)
            // Assume ANSI C
            #define ARENA_C 1989
        #elif __STDC_VERSION__ <= 199409L
            #define ARENA_C 1994
        #elif __STDC_VERSION__ <= 199901L
            #define ARENA_C 1999
        #elif __STDC_VERSION__ <= 201112L
            #define ARENA_C 2011
        #elif __STDC_VERSION__ <= 201710L
            #define ARENA_C 2017
        #else
            #define ARENA_C 2023
        #endif
    #endif
#else
    #error "Unsupported language"
#endif

#if defined(ARENA_C)
    #define ARENA_BEGIN_DECLS
    #define ARENA_END_DECLS
#elif defined(ARENA_CXX)
    #define ARENA_BEGIN_DECLS extern "C" {
    #define ARENA_END_DECLS }
#else
    #error "Unsupported language"
#endif

ARENA_BEGIN_DECLS

#define _GNU_SOURCE 1
#define _LARGEFILE_SOURCE 1
#define _LARGEFILE64_SOURCE 1
#define _FILE_OFFSET_BITS 64
#define _TIME_BITS 64
#if !defined(_FORTIFY_SOURCE)
    #define _FORTIFY_SOURCE 3
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define ARENA_ON_WINDOWS 1
#elif (defined(__linux__) || defined(__unix__)) && !(defined(__APPLE__) || defined(__ANDROID__))
    // Unix but not Apple or Android (i.e. Linux, BSD, Solaris, etc.)
    #define ARENA_ON_UNIX 1
#elif defined(__APPLE__)
    #define ARENA_ON_MACOS 1
#elif defined(__ANDROID__)
    #define ARENA_ON_ANDROID 1
#else
    #warning "Unknown platform, assuming Unix-like"
#endif

/*
 * The conditionnal below could be commented out, since (according to https://en.cppreference.com/w/c/atomic) :
 *     "If the macro constant __STDC_NO_ATOMICS__(C11) is defined by the compiler, the header <stdatomic.h>, 
 *      the keyword _Atomic [...] are not provided."
 * Thus, we shouldn't need to check for the C(++) version, but we'll do it anyway since for example some compilers
 * are not fully compliant with the standard and thus don't define __STDC_NO_ATOMICS__ even if they don't support
 * C(++)11 atomics (as another example, MinGW-w64 doesn't define __STDC_NO_THREADS__ even if it doesn't support them,
 * and doesn't provide <threads.h> (at least used to)).
 * 
*/

#if (defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011)
    #if defined(__STDC_NO_ATOMICS__)
        #define ARENA_HAS_ATOMIC 0
        #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
        #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
    #else
        #define ARENA_HAS_ATOMIC 1
        #define ARENA_ATOMIC_SPECIFIED_TYPE(type) _Atomic(type)
        #define ARENA_ATOMIC_QUALIFIED_TYPE(type) _Atomic type
        #include <stdatomic.h>
    #endif
    // For Windows, prefer the Windows thread API (even with Cygwin's GCC), wich we are sure is available
    #if defined(__STDC_NO_THREADS__) || defined(ARENA_ON_WINDOWS)
        #define ARENA_HAS_THREADS 0
    #else
        #define ARENA_HAS_THREADS 1
        #include <threads.h>
    #endif
    #include <stdalign.h>
    #define ARENA_ALIGNAS(x) alignas(x)
    #define ARENA_ALIGNOF(x) alignof(x)
#endif

#if defined(ARENA_ON_WINDOWS)
    #if ARENA_HAS_ATOMIC == 0
        // Use Interlocked* functions
        #define ARENA_HAS_ATOMIC 2
        #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
        #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
        #include <windows.h>
    #endif
    #if ARENA_HAS_THREADS == 0
        // Use Windows API
        #define ARENA_HAS_THREADS 2
        #include <windows.h>
    #endif
#endif

#if defined(ARENA_WITH_CLANG) && (ARENA_HAS_FEATURE(c_atomic) || ARENA_HAS_EXTENSION(c_atomic))
    #if ARENA_HAS_ATOMIC == 0
        #if ARENA_HAS_INCLUDE(<stdatomic.h>)
            #define ARENA_HAS_ATOMIC 1
            #define ARENA_ATOMIC_SPECIFIED_TYPE(type) _Atomic(type)
            #define ARENA_ATOMIC_QUALIFIED_TYPE(type) _Atomic type
            #include <stdatomic.h>
        #else
        // Use clang builtins
            #define ARENA_HAS_ATOMIC 3
            #define ARENA_ATOMIC_SPECIFIED_TYPE(type) _Atomic(type)
            #define ARENA_ATOMIC_QUALIFIED_TYPE(type) _Atomic type
        #endif
    #endif
    #if ARENA_HAS_THREADS == 0
        #if ARENA_HAS_INCLUDE(<threads.h>)
            #define ARENA_HAS_THREADS 1
            #include <threads.h>
        #elif ARENA_HAS_INCLUDE(<pthread.h>)
            #define ARENA_HAS_THREADS 3
            #include <pthread.h>
        #endif
    #endif
    #if !defined(ARENA_ALIGNAS)
        #define ARENA_ALIGNAS(x) __attribute__((__aligned__(x)))
    #endif
    #if !defined(ARENA_ALIGNOF)
        // Pray for it to be defined
        #define ARENA_ALIGNOF(x) _Alignof(x)
    #endif
#endif

#if defined(ARENA_WITH_GCC) && ((defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011))
    #if ARENA_HAS_ATOMIC == 0
    // Use gcc builtins, that needs C(++)11 memory model
        #define ARENA_HAS_ATOMIC 4
        #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
        #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
    #endif
#endif
#if defined(ARENA_WITH_GCC)
    #if ARENA_HAS_THREADS == 0
        #if ARENA_HAS_INCLUDE(<threads.h>)
            #define ARENA_HAS_THREADS 1
            #include <threads.h>
        #elif ARENA_HAS_INCLUDE(<pthread.h>)
            #define ARENA_HAS_THREADS 3
            #include <pthread.h>
        #endif
    #endif
    #if !defined(ARENA_ALIGNAS)
        #if ARENA_HAS_FEATURE(c_alignas) || ARENA_HAS_EXTENSION(c_alignas) || ARENA_HAS_INCLUDE(<stdalign.h>)
            #define ARENA_ALIGNAS(x) _Alignas(x)
        #else
            #define ARENA_ALIGNAS(x) __attribute__((__aligned__(x)))
        #endif
    #endif
    #if !defined(ARENA_ALIGNOF)
        #if ARENA_HAS_FEATURE(c_alignof) || ARENA_HAS_EXTENSION(c_alignof) || ARENA_HAS_INCLUDE(<stdalign.h>)
            #define ARENA_ALIGNOF(x) _Alignof(x)
        #else
            // Pray for it to work
            #define ARENA_ALIGNOF(x) __alignof__(x)
        #endif
    #endif
#endif

#if !defined(ARENA_ALIGNAS)
    #if defined(ARENA_WITH_MSVC)
        #define ARENA_ALIGNAS(x) __declspec(align(x))
    #else
        // Pray for it to be defined
        #define ARENA_ALIGNAS(x) _Alignas(x)
    #endif
#endif
#if !defined(ARENA_ALIGNOF)
    #if defined(ARENA_WITH_MSVC)
        #define ARENA_ALIGNOF(x) __alignof(x)
    #else
        // Pray for it to be defined
        #define ARENA_ALIGNOF(x) _Alignof(x)
    #endif
#endif

#if ARENA_HAS_ATOMIC == 0
    #if ARENA_HAS_INCLUDE(<stdatomic.h>)
        #define ARENA_HAS_ATOMIC 1
        // Having the include doesn't mean we have the _Atomic keyword
        #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
        #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
        #include <stdatomic.h>
    #endif
#endif

#if ARENA_HAS_THREADS == 0
    #if ARENA_HAS_INCLUDE(<threads.h>)
        #define ARENA_HAS_THREADS 1
        #include <threads.h>
    #elif ARENA_HAS_INCLUDE(<pthread.h>)
        #define ARENA_HAS_THREADS 3
        #include <pthread.h>
    #endif
#endif

#if ARENA_HAS_ATOMIC == 0 && ARENA_HAS_THREADS == 0
    #define ARENA_THREAD_SAFE 0
#else
    #define ARENA_THREAD_SAFE 1
#endif

#include <stdint.h>

#define ARENA_LOCK_INT_TYPE int16_t
#define ARENA_MK_LOCK_INT_TYPE(value) INT16_C(value)

#if defined(ARENA_CXX) && !defined(ARENA_C)
    #undef ARENA_ATOMIC_SPECIFIED_TYPE
    #undef ARENA_ATOMIC_QUALIFIED_TYPE
    #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
    #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
#endif

#if ARENA_THREAD_SAFE == 1
    #if ARENA_HAS_ATOMIC != 0
        #if ARENA_HAS_ATOMIC == 1
        // Use C(++)11 atomics
            #define ARENA_LOCK_TYPE volatile ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE)
            #define ARENA_LOCK_INIT(lock) atomic_init(&(lock), ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_DESTROY(lock) atomic_store_explicit(&(lock), ARENA_MK_LOCK_INT_TYPE(0), memory_order_relaxed)
            #define ARENA_LOCK(lock)                                        \
                {                                                           \
                    ARENA_LOCK_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);   \
                    ARENA_LOCK_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);    \
                    while(!atomic_compare_exchange_weak_explicit(           \
                          &(lock),                                          \
                          &expected,                                        \
                          desired,                                          \
                          memory_order_acquire,                             \
                          memory_order_relaxed))                            \
                    {                                                       \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);               \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                \
                    }                                                       \
                }
            #define ARENA_UNLOCK(lock)          \
                atomic_store_explicit(          \
                    &(lock),                    \
                    ARENA_MK_LOCK_INT_TYPE(0),  \
                    memory_order_release        \
                )
        #elif ARENA_HAS_ATOMIC == 2
        // Use Windows Interlocked* functions
            #define ARENA_LOCK_TYPE volatile ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE)
            #define ARENA_LOCK_INIT(lock) InterlockedExchange16(&(lock), ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_DESTROY(lock) InterlockedExchange16(&(lock), ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK(lock)                                        \
                {                                                           \
                    ARENA_LOCK_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);   \
                    ARENA_LOCK_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);    \
                    while(!InterlockedCompareExchange16(                    \
                            &(lock),                                        \
                            desired,                                        \
                            expected))                                      \
                    {                                                       \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);               \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                \
                    }                                                       \
                }
            #define ARENA_UNLOCK(lock) InterlockedExchange16(&(lock), ARENA_MK_LOCK_INT_TYPE(0))
        #elif ARENA_HAS_ATOMIC == 3
        // Use clang builtins
            #define ARENA_LOCK_TYPE volatile ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE)
            #define ARENA_LOCK_INIT(lock) __c11_atomic_store(&(lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK_DESTROY(lock) __c11_atomic_store(&(lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK(lock)                                        \
                {                                                           \
                    ARENA_LOCK_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);   \
                    ARENA_LOCK_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);    \
                    while(!__c11_atomic_compare_exchange_weak(              \
                            &(lock),                                        \
                            &expected,                                      \
                            desired,                                        \
                            __ATOMIC_ACQUIRE,                               \
                            __ATOMIC_RELAXED))                              \
                    {                                                       \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);               \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                \
                    }                                                       \
                }
            #define ARENA_UNLOCK(lock) __c11_atomic_store(&(lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELEASE)
        #elif ARENA_HAS_ATOMIC == 4
        // Use gcc builtins
            #define ARENA_LOCK_TYPE volatile ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE)
            #define ARENA_LOCK_INIT __atomic_store_n(&(lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK_DESTROY(lock) __atomic_store_n(&(lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK(lock)                                        \
                {                                                           \
                    ARENA_LOCK_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);   \
                    ARENA_LOCK_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);    \
                    while(!__atomic_compare_exchange_n(                     \
                            &(lock),                                        \
                            &expected,                                      \
                            desired,                                        \
                            1, /* weak */                                   \
                            __ATOMIC_ACQUIRE,                               \
                            __ATOMIC_RELAXED))                              \
                    {                                                       \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);               \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                \
                    }                                                       \
                }
        #endif
    #elif ARENA_HAS_THREADS == 1
    // Use C11 threads
        #define ARENA_LOCK_TYPE mtx_t
        #define ARENA_LOCK_INIT(lock) mtx_init(&(lock), mtx_plain)
        #define ARENA_LOCK_DESTROY(lock) mtx_destroy(&(lock))
        #define ARENA_LOCK(lock) mtx_lock(&(lock))
        #define ARENA_UNLOCK(lock) mtx_unlock(&(lock))
    #elif ARENA_HAS_THREADS == 2
    // Use Windows API
        #define ARENA_LOCK_TYPE CRITICAL_SECTION
        #define ARENA_LOCK_INIT(lock) InitializeCriticalSection(&(lock))
        #define ARENA_LOCK_DESTROY(lock) DeleteCriticalSection(&(lock))
        #define ARENA_LOCK(lock) EnterCriticalSection(&(lock))
        #define ARENA_UNLOCK(lock) LeaveCriticalSection(&(lock))
    #elif ARENA_HAS_THREADS == 3
    // Use pthread
        #define ARENA_LOCK_TYPE pthread_mutex_t
        #define ARENA_LOCK_INIT(lock) pthread_mutex_init(&(lock), NULL)
        #define ARENA_LOCK_DESTROY(lock) pthread_mutex_destroy(&(lock))
        #define ARENA_LOCK(lock) pthread_mutex_lock(&(lock))
        #define ARENA_UNLOCK(lock) pthread_mutex_unlock(&(lock))
    #endif
#else
    #include <signal.h>
    #define ARENA_LOCK_TYPE volatile sig_atomic_t
    #define ARENA_LOCK_INIT(lock) (lock) = 0
    #define ARENA_LOCK_DESTROY(lock) (lock) = 0
    #define ARENA_LOCK(lock)                \
        {                                   \
            ARENA_LOCK_TYPE expected = 0;   \
            ARENA_LOCK_TYPE desired = 1;    \
            while((lock) != expected)       \
            {                               \
                expected = 0;               \
                desired = 1;                \
            }                               \
            (lock) = 1;                     \
        }
    #define ARENA_UNLOCK(lock) (lock) = 0
#endif

#if !defined(ARENA_STATIC_CAP)
    #define ARENA_STATIC_CAP (1024 * 1024 * 10)
#endif

#include <assert.h>

#if !defined(ARENA_ASSERT)
    #define ARENA_ASSERT(x) assert(x)
#endif

static_assert((ARENA_STATIC_CAP >= 0) && (ARENA_STATIC_CAP % 10 == 0) && ((ARENA_STATIC_CAP / 10) % 8 == 0), "ARENA_STATIC_CAP must be a positive and divisible by 10 and (ARENA_STATIC_CAP / 10) must be divisible by 8 (ARENA_STATIC_CAP must be divisible by 80)");

#if !defined(ARENA_DEFAULT_ALIGN)
    #define ARENA_DEFAULT_ALIGN ARENA_ALIGNOF(max_align_t)
#endif

static_assert(ARENA_DEFAULT_ALIGN >= 1, "ARENA_DEFAULT_ALIGN must be greater than or equal to 1");

#define ARENA_BITMAP_SIZE (ARENA_STATIC_CAP / 80)

#if !defined(ARENA_STATIC_MEM_DEFAULT_ALIGN)
    #define ARENA_STATIC_MEM_DEFAULT_ALIGN 512
#endif

typedef uint8_t bit_map_t[ARENA_BITMAP_SIZE];
/*
 * TODO: Change `state` to store information about size of the allocated chunk to which belong
 * the bytes, so it's possible to free them.
 * TODO: Also, include a lock in the struct to make it thread-safe.
 */
typedef struct RawMemory
{
    ARENA_ALIGNAS(ARENA_STATIC_MEM_DEFAULT_ALIGN)
    char data[ARENA_STATIC_CAP / 10];
    bit_map_t state;
} raw_mem_t;

typedef struct Arena
{
    ARENA_LOCK_TYPE lock;
    struct Arena* next;
    char* avail;
    char* limit;
} arena_t;

#if defined(ARENA_DEBUG)
    #include <stdio.h>
    #include <stdbool.h>
bool bitmap_set(size_t start, size_t size, bit_map_t bitmap, bool value);
size_t bitmap_first_fit(size_t size, size_t align, bit_map_t bitmap);
void bitmap_print(FILE* stream, size_t start, size_t size, bit_map_t bitmap);
#endif

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
void arena_set_mmap_threshold(size_t size);
#endif
void arenalloc_init();
void arenalloc_deinit();

ARENA_END_DECLS

#endif // ARENALLOC_H
