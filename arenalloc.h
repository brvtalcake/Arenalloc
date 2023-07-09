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
#if defined(ARENA_ID)
    #undef ARENA_ID
#endif
#if defined(ARENA_ID_REDIRECT_0) || defined(ARENA_ID_REDIRECT_1) || defined(ARENA_ID_REDIRECT_2) \
 || defined(ARENA_ID_REDIRECT_3) || defined(ARENA_ID_REDIRECT_4) || defined(ARENA_ID_REDIRECT_5) \
 || defined(ARENA_ID_REDIRECT_6)
    #undef ARENA_ID_REDIRECT_0
    #undef ARENA_ID_REDIRECT_1
    #undef ARENA_ID_REDIRECT_2
    #undef ARENA_ID_REDIRECT_3
    #undef ARENA_ID_REDIRECT_4
    #undef ARENA_ID_REDIRECT_5
    #undef ARENA_ID_REDIRECT_6
#endif
#if defined(ARENA_ARGC)
    #undef ARENA_ARGC
#endif
#if defined(ARENA_ARGC_REDIRECT_0) || defined(ARENA_ARGC_REDIRECT_1) || defined(ARENA_ARGC_REDIRECT)
    #undef ARENA_ARGC_REDIRECT_0
    #undef ARENA_ARGC_REDIRECT_1
    #undef ARENA_ARGC_REDIRECT
#endif
#if defined(ARENA_ARGS_AUGMENTER)
    #undef ARENA_ARGS_AUGMENTER
#endif
#if defined(ARENA_EXPAND_ARGS)
    #undef ARENA_EXPAND_ARGS
#endif
#if defined(ARENA_EXPAND)
    #undef ARENA_EXPAND
#endif
#if defined(ARENA_NAT_EQ)
    #undef ARENA_NAT_EQ
#endif
#if defined(ARENA_NAT_EQ_0_0) || defined(ARENA_NAT_EQ_0_1) || defined(ARENA_NAT_EQ_1_0) \
 || defined(ARENA_NAT_EQ_0_2) || defined(ARENA_NAT_EQ_2_0) || defined(ARENA_NAT_EQ_1_1) \
 || defined(ARENA_NAT_EQ_1_2) || defined(ARENA_NAT_EQ_2_1) || defined(ARENA_NAT_EQ_2_2) \
 || defined(ARENA_NAT_EQ_0_3) || defined(ARENA_NAT_EQ_3_0) || defined(ARENA_NAT_EQ_1_3) \
 || defined(ARENA_NAT_EQ_3_1) || defined(ARENA_NAT_EQ_2_3) || defined(ARENA_NAT_EQ_3_2) \
 || defined(ARENA_NAT_EQ_3_3)
    #undef ARENA_NAT_EQ_0_0
    #undef ARENA_NAT_EQ_0_1
    #undef ARENA_NAT_EQ_1_0
    #undef ARENA_NAT_EQ_0_2
    #undef ARENA_NAT_EQ_2_0
    #undef ARENA_NAT_EQ_1_1
    #undef ARENA_NAT_EQ_1_2
    #undef ARENA_NAT_EQ_2_1
    #undef ARENA_NAT_EQ_2_2
    #undef ARENA_NAT_EQ_0_3
    #undef ARENA_NAT_EQ_3_0
    #undef ARENA_NAT_EQ_1_3
    #undef ARENA_NAT_EQ_3_1
    #undef ARENA_NAT_EQ_2_3
    #undef ARENA_NAT_EQ_3_2
    #undef ARENA_NAT_EQ_3_3
#endif
#if defined(ARENA_PP_IF) || defined(ARENA_PP_IF_0) || defined(ARENA_PP_IF_1)
    #undef ARENA_PP_IF
    #undef ARENA_PP_IF_0
    #undef ARENA_PP_IF_1
#endif
#if defined(ARENA_EAT)
    #undef ARENA_EAT
#endif
#if defined(ARENA_MACRO_HAS_NO_ARG)
    #undef ARENA_MACRO_HAS_NO_ARG
#endif
#if defined(ARENA_PP_NOT)
    #undef ARENA_PP_NOT
#endif
#if defined(ARENA_PP_AND)
    #undef ARENA_PP_AND
#endif
#if defined(ARENA_PP_OR)
    #undef ARENA_PP_OR
#endif
#if defined(ARENA_PP_XOR)
    #undef ARENA_PP_XOR
#endif
#if defined(ARENA_PP_NAND)
    #undef ARENA_PP_NAND
#endif
#if defined(ARENA_PP_NOR)
    #undef ARENA_PP_NOR
#endif
#if defined(ARENA_PP_XNOR)
    #undef ARENA_PP_XNOR
#endif
#if defined(ARENA_GET_ARG_N)
    #undef ARENA_GET_ARG_N
#endif
#if defined(ARENA_GET_ARG_0) || defined(ARENA_GET_ARG_1) || defined(ARENA_GET_ARG_2) || defined(ARENA_GET_ARG_3) \
 || defined(ARENA_GET_ARG_4)
    #undef ARENA_GET_ARG_0
    #undef ARENA_GET_ARG_1
    #undef ARENA_GET_ARG_2
    #undef ARENA_GET_ARG_3
    #undef ARENA_GET_ARG_4
#endif
#if defined(ARENA_PP_INVOKE)
    #undef ARENA_PP_INVOKE
#endif
#if defined(ARENA_PP_INVOKE_REDIRECT)
    #undef ARENA_PP_INVOKE_REDIRECT
#endif
#if defined(ARENA_FUNC_ARTIFICIAL)
    #undef ARENA_FUNC_ARTIFICIAL
#endif
#if defined(ARENA_FUNC_WUR)
    #undef ARENA_FUNC_WUR
#endif
#if defined(ARENA_FUNC_NONNULL)
    #undef ARENA_FUNC_NONNULL
#endif
#if defined(ARENA_FUNC_MALLOC)
    #undef ARENA_FUNC_MALLOC
#endif
#if defined(ARENA_FUNC_MALLOC_REDIRECT)
    #undef ARENA_FUNC_MALLOC_REDIRECT
#endif
#if defined(ARENA_FUNC_ALLOC_SIZE)
    #undef ARENA_FUNC_ALLOC_SIZE
#endif
#if defined(ARENA_FUNC_ALLOC_SIZE_REDIRECT)
    #undef ARENA_FUNC_ALLOC_SIZE_REDIRECT
#endif
#if defined(ARENA_FUNC_ALLOC_ALIGN)
    #undef ARENA_FUNC_ALLOC_ALIGN
#endif
#if defined(ARENA_ALLOCATOR)
    #undef ARENA_ALLOCATOR
#endif
#if defined(ARENA_ALLOCATOR_REDIRECT)
    #undef ARENA_ALLOCATOR_REDIRECT
#endif
#if defined(ARENA_FUNC_PURE)
    #undef ARENA_FUNC_PURE
#endif
#if defined(ARENA_FUNC_CONST)
    #undef ARENA_FUNC_CONST
#endif

#define ARENA_CAT_REDIRECT(x, y) x##y
#define ARENA_CAT(x, y) ARENA_CAT_REDIRECT(x, y)
#define ARENA_NAMESPACE(ident) ARENA_CAT(arena_, ident)
#define ARENA_ARGC(...) ARENA_ARGC_REDIRECT(__VA_ARGS__)
#define ARENA_ARGC_REDIRECT(...) ARENA_PP_IF(ARENA_MACRO_HAS_NO_ARG(__VA_ARGS__))(0)(ARENA_ARGC_REDIRECT_0(__VA_ARGS__))
#define ARENA_PP_INVOKE(macro, tupled_args) ARENA_PP_INVOKE_REDIRECT(macro, tupled_args)
#define ARENA_PP_INVOKE_REDIRECT(macro, tupled_args) macro tupled_args
#define ARENA_GET_ARG_N(n, tupled_args) ARENA_PP_INVOKE(ARENA_ID(ARENA_CAT(ARENA_GET_ARG_, n)), tupled_args)
#define ARENA_GET_ARG_0(_0, ...) _0
#define ARENA_GET_ARG_1(_0, _1, ...) _1
#define ARENA_GET_ARG_2(_0, _1, _2, ...) _2
#define ARENA_GET_ARG_3(_0, _1, _2, _3, ...) _3
#define ARENA_GET_ARG_4(_0, _1, _2, _3, _4, ...) _4

// Just to be sure VSCode won't crash (yes it crashes on my computer when I use normal ARENA_ID)
#if defined(__INTELLISENSE__)
    #define ARENA_ID(...) ARENA_ID_REDIRECT_0(__VA_ARGS__)
    #define ARENA_ID_REDIRECT_0(...) __VA_ARGS__
#else
    #define ARENA_ID(...) ARENA_ID_REDIRECT_0(ARENA_ID_REDIRECT_0(ARENA_ID_REDIRECT_0(ARENA_ID_REDIRECT_0(ARENA_ID_REDIRECT_0(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_0(...) ARENA_ID_REDIRECT_1(ARENA_ID_REDIRECT_1(ARENA_ID_REDIRECT_1(ARENA_ID_REDIRECT_1(ARENA_ID_REDIRECT_1(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_1(...) ARENA_ID_REDIRECT_2(ARENA_ID_REDIRECT_2(ARENA_ID_REDIRECT_2(ARENA_ID_REDIRECT_2(ARENA_ID_REDIRECT_2(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_2(...) ARENA_ID_REDIRECT_3(ARENA_ID_REDIRECT_3(ARENA_ID_REDIRECT_3(ARENA_ID_REDIRECT_3(ARENA_ID_REDIRECT_3(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_3(...) ARENA_ID_REDIRECT_4(ARENA_ID_REDIRECT_4(ARENA_ID_REDIRECT_4(ARENA_ID_REDIRECT_4(ARENA_ID_REDIRECT_4(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_4(...) ARENA_ID_REDIRECT_5(ARENA_ID_REDIRECT_5(ARENA_ID_REDIRECT_5(ARENA_ID_REDIRECT_5(ARENA_ID_REDIRECT_5(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_5(...) ARENA_ID_REDIRECT_6(ARENA_ID_REDIRECT_6(ARENA_ID_REDIRECT_6(ARENA_ID_REDIRECT_6(ARENA_ID_REDIRECT_6(__VA_ARGS__)))))
    #define ARENA_ID_REDIRECT_6(...) __VA_ARGS__
#endif

#define ARENA_EXPAND(x) ARENA_ID(x)

#if defined(_MSC_VER)
    #define ARENA_ARGC_REDIRECT_0(...)  ARENA_EXPAND_ARGS(ARENA_ARGS_AUGMENTER(__VA_ARGS__))
    #define ARENA_ARGS_AUGMENTER(...) unused, __VA_ARGS__
    #define ARENA_EXPAND_ARGS(...) ARENA_EXPAND(ARENA_ARGC_REDIRECT_1(__VA_ARGS__, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
    #define ARENA_ARGC_REDIRECT_1(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count
#else
    #define ARENA_ARGC_REDIRECT_0(...)                                          \
        ARENA_ARGC_REDIRECT_1(                                                  \
            0, ## __VA_ARGS__, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60,      \
            59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44,     \
            43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28,     \
            27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,     \
            11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0                                \
        )
    #define ARENA_ARGC_REDIRECT_1(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count
#endif

#define ARENA_EAT(...)
#define ARENA_PP_IF(cond) ARENA_CAT(ARENA_PP_IF_, cond)
#define ARENA_PP_IF_0(...) ARENA_ID
#define ARENA_PP_IF_1(...) __VA_ARGS__ ARENA_EAT
#define ARENA_NAT_EQ(x, y) ARENA_CAT(ARENA_NAT_EQ_, ARENA_CAT(x, ARENA_CAT(_, y)))
#define ARENA_NAT_EQ_0_0 1
#define ARENA_NAT_EQ_0_1 0
#define ARENA_NAT_EQ_1_0 0
#define ARENA_NAT_EQ_0_2 0
#define ARENA_NAT_EQ_2_0 0
#define ARENA_NAT_EQ_0_3 0
#define ARENA_NAT_EQ_3_0 0
#define ARENA_NAT_EQ_1_1 1
#define ARENA_NAT_EQ_1_2 0
#define ARENA_NAT_EQ_2_1 0
#define ARENA_NAT_EQ_1_3 0
#define ARENA_NAT_EQ_3_1 0
#define ARENA_NAT_EQ_2_2 1
#define ARENA_NAT_EQ_2_3 0
#define ARENA_NAT_EQ_3_2 0
#define ARENA_NAT_EQ_3_3 1
#define ARENA_PP_NOT(x) ARENA_NAT_EQ(x, 0)
#define ARENA_PP_AND(x, y) ARENA_PP_IF(x)(ARENA_NAT_EQ(y, 1))(0)
#define ARENA_PP_OR(x, y) ARENA_PP_IF(x)(1)(y)
#define ARENA_PP_XOR(x, y) ARENA_PP_IF(x)(ARENA_PP_NOT(y))(y)
#define ARENA_PP_NAND(x, y) ARENA_PP_IF(x)(ARENA_PP_NOT(y))(1)
#define ARENA_PP_NOR(x, y) ARENA_PP_IF(x)(0)(ARENA_PP_NOT(y))
#define ARENA_PP_XNOR(x, y) ARENA_PP_IF(x)(y)(ARENA_PP_NOT(y))

#define ARENA_MACRO_HAS_NO_ARG(...) ISEMPTY(__VA_ARGS__)

/* 
 * Macros taken from  ̶J̶e̶n̶s̶ ̶G̶u̶s̶t̶e̶d̶t̶'̶s̶ THE GOAT's blog :
 * (https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/) 
 */
#ifndef JENS_PP_MACROS
#define JENS_PP_MACROS

#if defined(_ARG16)
    #undef _ARG16
#endif
#if defined(HAS_COMMA)
    #undef HAS_COMMA
#endif
#if defined(_TRIGGER_PARENTHESIS_)
    #undef _TRIGGER_PARENTHESIS_
#endif
#if defined(ISEMPTY)
    #undef ISEMPTY
#endif
#if defined(PASTE5)
    #undef PASTE5
#endif
#if defined(_ISEMPTY)
    #undef _ISEMPTY
#endif
#if defined(_IS_EMPTY_CASE_)
    #undef _IS_EMPTY_CASE_
#endif
#if defined(_IS_EMPTY_CASE_0001)
    #undef _IS_EMPTY_CASE_0001
#endif
#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) _ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _TRIGGER_PARENTHESIS_(...) ,
 
#define ISEMPTY(...)                                                    \
_ISEMPTY(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          HAS_COMMA(__VA_ARGS__ (/*empty*/)),                           \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/))      \
          )
 
#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define _ISEMPTY(_0, _1, _2, _3) HAS_COMMA(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _IS_EMPTY_CASE_0001 ,

#endif // JENS_PP_MACROS

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

#if ARENA_THREAD_SAFE == 1
    #define ARENA_LOCK_INT_TYPE int16_t
    #define ARENA_MK_LOCK_INT_TYPE(value) INT16_C(value)
#else
    #define ARENA_LOCK_INT_TYPE sig_atomic_t
    #define ARENA_MK_LOCK_INT_TYPE(value) value
#endif
#if defined(ARENA_CXX) && !defined(ARENA_C)
    #undef ARENA_ATOMIC_SPECIFIED_TYPE
    #undef ARENA_ATOMIC_QUALIFIED_TYPE
    #define ARENA_ATOMIC_SPECIFIED_TYPE(type) type
    #define ARENA_ATOMIC_QUALIFIED_TYPE(type) type
#endif

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
    #include <unistd.h>
    #include <sys/types.h>
    #define ARENA_TID_TYPE pid_t
    #define ARENA_TID gettid()
#elif defined(ARENA_ON_WINDOWS)
    #include <windows.h>
    #define ARENA_TID_TYPE DWORD
    #define ARENA_TID GetCurrentThreadId()
#endif


// TODO: Finish these macros
#if ARENA_THREAD_SAFE == 1
    #if ARENA_HAS_ATOMIC != 0
        #if ARENA_HAS_ATOMIC == 1
        // Use C(++)11 atomics
            typedef struct ArenaLock
            {
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE) main_lock;
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_TID_TYPE) tid;
            } arena_lock_t;
            #define ARENA_LOCK_TYPE volatile arena_lock_t
            #define ARENA_LOCK_INIT(lock) atomic_init(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0)); atomic_init(&((lock).tid), ARENA_TID)
            #define ARENA_LOCK_DESTROY(lock) atomic_store_explicit(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), memory_order_relaxed)
            #define ARENA_LOCK(lock)                                                        \
                {                                                                           \
                    volatile ARENA_LOCK_INT_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);      \
                    volatile ARENA_LOCK_INT_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);       \
                    while(!atomic_compare_exchange_weak_explicit(                           \
                          &((lock).main_lock),                                              \
                          &expected,                                                        \
                          desired,                                                          \
                          memory_order_acquire,                                             \
                          memory_order_relaxed))                                            \
                    {                                                                       \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);                               \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                                \
                    }                                                                       \
                    atomic_store_explicit(&((lock).tid), ARENA_TID, memory_order_acquire);  \
                }
            #define ARENA_UNLOCK(lock)          \
                atomic_store_explicit(          \
                    &((lock).main_lock),        \
                    ARENA_MK_LOCK_INT_TYPE(0),  \
                    memory_order_release        \
                )
            #define ARENA_LOCK_IS_LOCKED(lock) (atomic_load_explicit(&((lock).main_lock), memory_order_relaxed) != ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_IS_MINE(lock) (atomic_load_explicit(&((lock).tid), memory_order_relaxed) == ARENA_TID)
        #elif ARENA_HAS_ATOMIC == 2
        // Use Windows Interlocked* functions
            typedef struct ArenaLock
            {
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE) main_lock;
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_TID_TYPE) tid;
            } arena_lock_t;
            #define ARENA_LOCK_TYPE volatile arena_lock_t
            #define ARENA_LOCK_INIT(lock) InterlockedExchange16(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0)); InterlockedExchange(&((lock).tid), ARENA_TID)
            #define ARENA_LOCK_DESTROY(lock) InterlockedExchange16(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK(lock)                                                    \
                {                                                                       \
                    volatile ARENA_LOCK_INT_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);  \
                    volatile ARENA_LOCK_INT_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);   \
                    while(!InterlockedCompareExchange16(                                \
                            &((lock).main_lock),                                        \
                            desired,                                                    \
                            expected))                                                  \
                    {                                                                   \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);                           \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                            \
                    }                                                                   \
                    InterlockedExchange(&((lock).tid), ARENA_TID);                      \
                }
            #define ARENA_UNLOCK(lock) InterlockedExchange16(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_IS_LOCKED(lock) (InterlockedOr16(&((lock).main_lock), 0) != ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_IS_MINE(lock) (InterlockedOr16(&((lock).tid), 0) == ARENA_TID)
        #elif ARENA_HAS_ATOMIC == 3
        // Use clang builtins
            typedef struct ArenaLock
            {
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE) main_lock;
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_TID_TYPE) tid;
            } arena_lock_t;
            #define ARENA_LOCK_TYPE volatile arena_lock_t
            #define ARENA_LOCK_INIT(lock) __c11_atomic_store(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED); __c11_atomic_store(&((lock).tid), ARENA_TID, __ATOMIC_RELAXED)
            #define ARENA_LOCK_DESTROY(lock) __c11_atomic_store(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK(lock)                                                    \
                {                                                                       \
                    volatile ARENA_LOCK_INT_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);  \
                    volatile ARENA_LOCK_INT_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);   \
                    while(!__c11_atomic_compare_exchange_weak(                          \
                            &((lock).main_lock),                                        \
                            &expected,                                                  \
                            desired,                                                    \
                            __ATOMIC_ACQUIRE,                                           \
                            __ATOMIC_RELAXED))                                          \
                    {                                                                   \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);                           \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                            \
                    }                                                                   \
                    __c11_atomic_store(&((lock).tid), ARENA_TID, __ATOMIC_ACQUIRE);     \
                }
            #define ARENA_UNLOCK(lock) __c11_atomic_store(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELEASE)
            #define ARENA_LOCK_IS_LOCKED(lock) (__c11_atomic_load(&((lock).main_lock), __ATOMIC_RELAXED) != ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_IS_MINE(lock) (__c11_atomic_load(&((lock).tid), __ATOMIC_RELAXED) == ARENA_TID)
        #elif ARENA_HAS_ATOMIC == 4
        // Use gcc builtins
            typedef struct ArenaLock
            {
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_LOCK_INT_TYPE) main_lock;
                ARENA_ATOMIC_QUALIFIED_TYPE(ARENA_TID_TYPE) tid;
            } arena_lock_t;
            #define ARENA_LOCK_TYPE volatile arena_lock_t
            #define ARENA_LOCK_INIT __atomic_store_n(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED); __atomic_store_n(&((lock).tid), ARENA_TID, __ATOMIC_RELAXED)
            #define ARENA_LOCK_DESTROY(lock) __atomic_store_n(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELAXED)
            #define ARENA_LOCK(lock)                                                    \
                {                                                                       \
                    volatile ARENA_LOCK_INT_TYPE expected = ARENA_MK_LOCK_INT_TYPE(0);  \
                    volatile ARENA_LOCK_INT_TYPE desired = ARENA_MK_LOCK_INT_TYPE(1);   \
                    while(!__atomic_compare_exchange_n(                                 \
                            &((lock).main_lock),                                        \
                            &expected,                                                  \
                            desired,                                                    \
                            1, /* weak */                                               \
                            __ATOMIC_ACQUIRE,                                           \
                            __ATOMIC_RELAXED))                                          \
                    {                                                                   \
                        expected = ARENA_MK_LOCK_INT_TYPE(0);                           \
                        desired = ARENA_MK_LOCK_INT_TYPE(1);                            \
                    }                                                                   \
                    __atomic_store_n(&((lock).tid), ARENA_TID, __ATOMIC_ACQUIRE);       \
                }
            #define ARENA_UNLOCK(lock) __atomic_store_n(&((lock).main_lock), ARENA_MK_LOCK_INT_TYPE(0), __ATOMIC_RELEASE)
            #define ARENA_LOCK_IS_LOCKED(lock) (__atomic_load_n(&((lock).main_lock), __ATOMIC_RELAXED) != ARENA_MK_LOCK_INT_TYPE(0))
            #define ARENA_LOCK_IS_MINE(lock) (__atomic_load_n(&((lock).tid), __ATOMIC_RELAXED) == ARENA_TID)
        #endif
    #elif ARENA_HAS_THREADS == 1
    // Use C11 threads
        typedef struct ArenaLock
        {
            mtx_t main_lock;
            mtx_t state_lock;
            mtx_t tid_lock;
            volatile ARENA_LOCK_INT_TYPE state;
            volatile ARENA_TID_TYPE tid;
        } arena_lock_t;
        #define ARENA_LOCK_TYPE arena_lock_t
        #define ARENA_LOCK_INIT(lock)                                               \
            {                                                                       \
                if (mtx_init(&((lock).main_lock), mtx_plain) != thrd_success)       \
                {                                                                   \
                    fprintf(stderr, "mtx_init failed\n");                           \
                    exit(EXIT_FAILURE);                                             \
                }                                                                   \
                if (mtx_init(&((lock).state_lock), mtx_recursive) != thrd_success)  \
                {                                                                   \
                    fprintf(stderr, "mtx_init failed\n");                           \
                    exit(EXIT_FAILURE);                                             \
                }                                                                   \
                if (mtx_init(&((lock).tid_lock), mtx_recursive) != thrd_success)    \
                {                                                                   \
                    fprintf(stderr, "mtx_init failed\n");                           \
                    exit(EXIT_FAILURE);                                             \
                }                                                                   \
                (lock).state = ARENA_MK_LOCK_INT_TYPE(0);                           \
                (lock).tid = ARENA_TID;                                             \
            }
        #define ARENA_LOCK_DESTROY(lock)            \
            {                                       \
                mtx_destroy(&((lock).main_lock));   \
                mtx_destroy(&((lock).state_lock));  \
                mtx_destroy(&((lock).tid_lock));    \
            }
        #define ARENA_LOCK(lock)                                        \
            {                                                           \
                if (mtx_lock(&((lock).main_lock)) != thrd_success)      \
                {                                                       \
                    fprintf(stderr, "mtx_lock failed\n");               \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                if (mtx_lock(&((lock).state_lock)) != thrd_success)     \
                {                                                       \
                    fprintf(stderr, "mtx_lock failed\n");               \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                (lock).state = ARENA_MK_LOCK_INT_TYPE(1);               \
                if (mtx_unlock(&((lock).state_lock)) != thrd_success)   \
                {                                                       \
                    fprintf(stderr, "mtx_unlock failed\n");             \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                if (mtx_lock(&((lock).tid_lock)) != thrd_success)       \
                {                                                       \
                    fprintf(stderr, "mtx_lock failed\n");               \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                (lock).tid = ARENA_TID;                                 \
                if (mtx_unlock(&((lock).tid_lock)) != thrd_success)     \
                {                                                       \
                    fprintf(stderr, "mtx_unlock failed\n");             \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
            }
        #define ARENA_UNLOCK(lock)                                      \
            {                                                           \
                if (mtx_lock(&((lock).state_lock)) != thrd_success)     \
                {                                                       \
                    fprintf(stderr, "mtx_lock failed\n");               \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                (lock).state = ARENA_MK_LOCK_INT_TYPE(0);               \
                if (mtx_unlock(&((lock).state_lock)) != thrd_success)   \
                {                                                       \
                    fprintf(stderr, "mtx_unlock failed\n");             \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
                if (mtx_unlock(&((lock).main_lock)) != thrd_success)    \
                {                                                       \
                    fprintf(stderr, "mtx_unlock failed\n");             \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
            }
        #define ARENA_LOCK_IS_LOCKED(lock) ((lock).state != ARENA_MK_LOCK_INT_TYPE(0))
        #define ARENA_LOCK_IS_MINE(lock) ((lock).tid == ARENA_TID)
    #elif ARENA_HAS_THREADS == 2
    // Use Windows API
        typedef struct ArenaLock
        {
            CRITICAL_SECTION main_lock;
            volatile ARENA_LOCK_INT_TYPE state;
            volatile ARENA_TID_TYPE tid;
        } arena_lock_t;
        #define ARENA_LOCK_TYPE arena_lock_t
        #define ARENA_LOCK_INIT(lock)                       \
        {                                                   \
            InitializeCriticalSection(&((lock).main_lock)); \
            (lock).state = ARENA_MK_LOCK_INT_TYPE(0);       \
            (lock).tid = ARENA_TID;                         \
        }
        #define ARENA_LOCK_DESTROY(lock) DeleteCriticalSection(&((lock).main_lock))
        #define ARENA_LOCK(lock)                                                \
        {                                                                       \
            InterlockedExchange16(&((lock).state), ARENA_MK_LOCK_INT_TYPE(1));  \
            InterlockedExchange(&((lock).tid), ARENA_TID);                      \
            EnterCriticalSection(&((lock).main_lock));                          \
        }
        #define ARENA_UNLOCK(lock)                                              \
        {                                                                       \
            LeaveCriticalSection(&((lock).main_lock));                          \
            InterlockedExchange16(&((lock).state), ARENA_MK_LOCK_INT_TYPE(0));  \
        }
        #define ARENA_LOCK_IS_LOCKED(lock) (InterlockedCompareExchange16(&((lock).state), ARENA_MK_LOCK_INT_TYPE(0), ARENA_MK_LOCK_INT_TYPE(0)) != ARENA_MK_LOCK_INT_TYPE(0))
        #define ARENA_LOCK_IS_MINE(lock) (InterlockedCompareExchange(&((lock).tid), ARENA_TID, ARENA_TID) == ARENA_TID)
    #elif ARENA_HAS_THREADS == 3
    // Use pthread
        typedef struct ArenaLock
        {
            pthread_mutex_t main_lock;
            pthread_mutex_t state_lock;
            pthread_mutex_t tid_lock;
            volatile ARENA_LOCK_INT_TYPE state;
            volatile ARENA_TID_TYPE tid;
        } arena_lock_t;
        #define ARENA_LOCK_TYPE arena_lock_t
        #define ARENA_LOCK_INIT(lock)                                           \
        {                                                                       \
            pthread_mutexattr_t attr;                                           \
            if (pthread_mutexattr_init(&attr) != 0)                             \
            {                                                                   \
                fprintf(stderr, "pthread_mutexattr_init failed\n");             \
                exit(EXIT_FAILURE);                                             \
            }                                                                   \
            if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) \
            {                                                                   \
                fprintf(stderr, "pthread_mutexattr_settype failed\n");          \
                exit(EXIT_FAILURE);                                             \
            }                                                                   \
            pthread_mutex_init(&((lock).main_lock), NULL);                      \
            pthread_mutex_init(&((lock).state_lock), &attr);                    \
            pthread_mutex_init(&((lock).tid_lock), &attr);                      \
            pthread_mutexattr_destroy(&attr);                                   \
            (lock).state = ARENA_MK_LOCK_INT_TYPE(0);                           \
            (lock).tid = ARENA_TID;                                             \
        }
        #define ARENA_LOCK_DESTROY(lock)                    \
        {                                                   \
            pthread_mutex_destroy(&((lock).main_lock));     \
            pthread_mutex_destroy(&((lock).state_lock));    \
            pthread_mutex_destroy(&((lock).tid_lock));      \
        }
        #define ARENA_LOCK(lock)                            \
        {                                                   \
            pthread_mutex_lock(&((lock).main_lock));        \
            pthread_mutex_lock(&((lock).state_lock));       \
            (lock).state = ARENA_MK_LOCK_INT_TYPE(1);       \
            pthread_mutex_unlock(&((lock).state_lock));     \
            pthread_mutex_lock(&((lock).tid_lock));         \
            (lock).tid = ARENA_TID;                         \
            pthread_mutex_unlock(&((lock).tid_lock));       \
        }
        #define ARENA_UNLOCK(lock)                          \
        {                                                   \
            pthread_mutex_lock(&((lock).state_lock));       \
            (lock).state = ARENA_MK_LOCK_INT_TYPE(0);       \
            pthread_mutex_unlock(&((lock).state_lock));     \
            pthread_mutex_unlock(&(lock).main_lock);        \
        }
        #define ARENA_LOCK_IS_LOCKED(lock) ((lock).state != ARENA_MK_LOCK_INT_TYPE(0))
        #define ARENA_LOCK_IS_MINE(lock) ((lock).tid == ARENA_TID)
    #endif
#else
    #include <signal.h>
    typedef ArenaLock
    {
        sig_atomic_t main_lock;
        ARENA_TID_TYPE tid;
    } arena_lock_t;
    #define ARENA_LOCK_TYPE volatile arena_lock_t
    #define ARENA_LOCK_INIT(lock) (lock) = { .main_lock = 0, .tid = ARENA_TID}
    #define ARENA_LOCK_DESTROY(lock) (lock) = { .main_lock = 0, .tid = ARENA_TID}
    #define ARENA_LOCK(lock)                    \
        {                                       \
            volatile sig_atomic_t expected = 0; \
            volatile sig_atomic_t desired = 1;  \
            while((lock).main_lock != expected) \
            {                                   \
                expected = 0;                   \
                desired = 1;                    \
            }                                   \
            (lock).tid = ARENA_TID;             \
            (lock).main_lock = 1;               \
        }
    #define ARENA_UNLOCK(lock) (lock).main_lock = 0
    #define ARENA_LOCK_IS_LOCKED(lock) ((lock).main_lock != 0)
    #define ARENA_LOCK_IS_MINE(lock) ((lock).tid == ARENA_TID)
#endif

#if defined(ARENA_WITH_GCC) || defined(ARENA_WITH_CLANG)
    #define ARENA_FUNC_ARTIFICIAL __attribute__((__artificial__))
    #define ARENA_FUNC_WUR __attribute__((__warn_unused_result__))
    #define ARENA_FUNC_MALLOC(...) ARENA_FUNC_MALLOC_REDIRECT(__VA_ARGS__)
    #define ARENA_FUNC_MALLOC_REDIRECT(...)                                             \
        ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 0))                           \
        (__attribute__((__malloc__)))                                                   \
        (                                                                               \
            ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 1))                       \
            (__attribute__((                                                            \
                __malloc__, __malloc__(ARENA_GET_ARG_N(0, (__VA_ARGS__))))))            \
            (                                                                           \
                ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 2))                   \
                (__attribute__((                                                        \
                    __malloc__,                                                         \
                    __malloc__(                                                         \
                        ARENA_GET_ARG_N(0, (__VA_ARGS__)),                              \
                        ARENA_GET_ARG_N(1, (__VA_ARGS__))                               \
                        )                                                               \
                    )                                                                   \
                ))                                                                      \
                (ARENA_STATIC_ASSERT(0, "ARENA_FUNC_MALLOC takes 0, 1 or 2 arguments")) \
            )                                                                           \
        )
    #define ARENA_FUNC_ALLOC_SIZE(...) ARENA_FUNC_ALLOC_SIZE_REDIRECT(__VA_ARGS__)
    #define ARENA_FUNC_ALLOC_SIZE_REDIRECT(...)                                         \
        ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 1))                           \
        (__attribute__((__alloc_size__(ARENA_GET_ARG_N(0, (__VA_ARGS__))))))            \
        (                                                                               \
            ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 2))                       \
            (__attribute__((                                                            \
                __alloc_size__(                                                         \
                    ARENA_GET_ARG_N(0, (__VA_ARGS__)),                                  \
                    ARENA_GET_ARG_N(1, (__VA_ARGS__))                                   \
                    )                                                                   \
                )                                                                       \
            ))                                                                          \
            (ARENA_STATIC_ASSERT(0, "ARENA_FUNC_ALLOC_SIZE takes 1 or 2 arguments"))    \
        )
    #define ARENA_FUNC_ALLOC_ALIGN(align) __attribute__((__alloc_align__(align)))
    #define ARENA_ALLOCATOR(...) ARENA_ALLOCATOR_REDIRECT(__VA_ARGS__)
    #define ARENA_ALLOCATOR_REDIRECT(...)                                                       \
        ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 0))                                   \
        (ARENA_FUNC_MALLOC())                                                                   \
        (                                                                                       \
            ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 1))                               \
            (ARENA_FUNC_MALLOC(ARENA_ID ARENA_GET_ARG_N(0, (__VA_ARGS__))))                     \
            (                                                                                   \
                ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 2))                           \
                (                                                                               \
                    ARENA_FUNC_MALLOC(ARENA_ID ARENA_GET_ARG_N(0, (__VA_ARGS__)))               \
                    ARENA_FUNC_ALLOC_SIZE(ARENA_ID ARENA_GET_ARG_N(1, (__VA_ARGS__)))           \
                )                                                                               \
                (                                                                               \
                    ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 3))                       \
                    (                                                                           \
                        ARENA_FUNC_MALLOC(ARENA_ID ARENA_GET_ARG_N(0, (__VA_ARGS__)))           \
                        ARENA_FUNC_ALLOC_SIZE(ARENA_ID ARENA_GET_ARG_N(1, (__VA_ARGS__)))       \
                        ARENA_FUNC_ALLOC_ALIGN(ARENA_GET_ARG_N(2, (__VA_ARGS__)))               \
                    )                                                                           \
                    (ARENA_STATIC_ASSERT(0, "ARENA_ALLOCATOR takes 0, 1, 2 or 3 arguments"))    \
                )                                                                               \
            )                                                                                   \
        )
        #define ARENA_FUNC_PURE __attribute__((__pure__))
        #define ARENA_FUNC_CONST __attribute__((__const__))
#else
    #define ARENA_FUNC_ARTIFICIAL
    #define ARENA_FUNC_WUR
    #define ARENA_FUNC_MALLOC(...) ARENA_FUNC_MALLOC_REDIRECT(__VA_ARGS__)
    #define ARENA_FUNC_MALLOC_REDIRECT(...) ARENA_EAT(__VA_ARGS__)
    #define ARENA_FUNC_ALLOC_SIZE(...) ARENA_FUNC_ALLOC_SIZE_REDIRECT(__VA_ARGS__)
    #define ARENA_FUNC_ALLOC_SIZE_REDIRECT(...) ARENA_EAT(__VA_ARGS__)
    #define ARENA_FUNC_ALLOC_ALIGN(align) ARENA_EAT(align)
    #define ARENA_ALLOCATOR(...) ARENA_ALLOCATOR_REDIRECT(__VA_ARGS__)
    #define ARENA_ALLOCATOR_REDIRECT(...) ARENA_EAT(__VA_ARGS__)
    #define ARENA_FUNC_PURE
    #define ARENA_FUNC_CONST
#endif

#if !defined(ARENA_STATIC_CAP)
    #define ARENA_STATIC_CAP (1024 * 1024 * 10)
#endif

#include <assert.h>

#if !defined(ARENA_ASSERT)
    #define ARENA_ASSERT(x) assert(x)
#endif
#if defined(ARENA_NO_STATIC_ASSERT)
    #define ARENA_STATIC_ASSERT(x, msg)
#endif
#if !defined(ARENA_STATIC_ASSERT)
    #if ((defined(ARENA_C) && ARENA_C >= 2011) || (defined(ARENA_CXX) && ARENA_CXX >= 2011))
        #define ARENA_STATIC_ASSERT(x, msg) static_assert(x, msg)
    #else
        #if defined(ARENA_C)
            #error "Your C standard doesn't support static_assert, please define ARENA_STATIC_ASSERT yourself, with the following signature : ARENA_STATIC_ASSERT(x, msg). Else, define ARENA_NO_STATIC_ASSERT to disable static asserts."
        #else
            #error "Your C++ standard doesn't support static_assert, please define ARENA_STATIC_ASSERT yourself, with the following signature : ARENA_STATIC_ASSERT(x, msg). Else, define ARENA_NO_STATIC_ASSERT to disable static asserts."
        #endif
    #endif
#endif

ARENA_STATIC_ASSERT((ARENA_STATIC_CAP >= 0) && (ARENA_STATIC_CAP % 10 == 0) && ((ARENA_STATIC_CAP / 10) % 8 == 0), "ARENA_STATIC_CAP must be a positive and divisible by 10 and (ARENA_STATIC_CAP / 10) must be divisible by 8 (ARENA_STATIC_CAP must be divisible by 80)");

#if !defined(ARENA_DEFAULT_ALIGN)
    #define ARENA_DEFAULT_ALIGN ARENA_ALIGNOF(max_align_t)
#endif

ARENA_STATIC_ASSERT(ARENA_DEFAULT_ALIGN >= 1, "ARENA_DEFAULT_ALIGN must be greater than or equal to 1");

#define ARENA_BITMAP_SIZE (ARENA_STATIC_CAP / 80)

#if defined(ARENA_STATIC_MEM_BASE_ALIGN) && !defined(ARENA_STATIC_MEM_MAX_ALIGN_REQUEST)
    #define ARENA_STATIC_MEM_MAX_ALIGN_REQUEST ARENA_STATIC_MEM_BASE_ALIGN
#elif !defined(ARENA_STATIC_MEM_BASE_ALIGN) && defined(ARENA_STATIC_MEM_MAX_ALIGN_REQUEST)
    #define ARENA_STATIC_MEM_BASE_ALIGN ARENA_STATIC_MEM_MAX_ALIGN_REQUEST
#elif !defined(ARENA_STATIC_MEM_BASE_ALIGN) && !defined(ARENA_STATIC_MEM_MAX_ALIGN_REQUEST)
    #define ARENA_STATIC_MEM_BASE_ALIGN 512
    #define ARENA_STATIC_MEM_MAX_ALIGN_REQUEST ARENA_STATIC_MEM_BASE_ALIGN
#else
    ARENA_STATIC_ASSERT(ARENA_STATIC_MEM_BASE_ALIGN >= ARENA_STATIC_MEM_MAX_ALIGN_REQUEST, "ARENA_STATIC_MEM_BASE_ALIGN must be greater than or equal to ARENA_STATIC_MEM_MAX_ALIGN_REQUEST");
#endif

typedef uint8_t bit_map_t[ARENA_BITMAP_SIZE];
/*
 * TODO: Change `state` to store information about size of the allocated chunk to which belong
 * the bytes, so it's possible to free them.
 * TODO: Also, include a lock in the struct to make it thread-safe.
 */
typedef struct RawMemory
{
    ARENA_ALIGNAS(ARENA_STATIC_MEM_BASE_ALIGN)
    char data[ARENA_STATIC_CAP / 10];
    bit_map_t state;
    size_t* region_sizes;
    size_t region_count;
} raw_mem_t;

typedef struct Arena
{
    struct Arena* next;
    char* avail;
    char* limit;
    ARENA_LOCK_TYPE lock;
} arena_t;

#if defined(ARENA_DEBUG)
    #include <stdio.h>
    #include <stdbool.h>
bool bitmap_set(size_t start, size_t size, bit_map_t bitmap, bool value);
size_t bitmap_first_fit(size_t size, size_t align, bit_map_t bitmap);
void bitmap_print(FILE* stream, size_t start, size_t size, bit_map_t bitmap);
ARENA_ALLOCATOR((), (1), 2)
void* arena_raw_mem_alloc_aligned(size_t size, size_t align);
void* arena_raw_mem_alloc_default(size_t size);
#if defined(arena_raw_mem_alloc)
    #undef arena_raw_mem_alloc
#endif
#define arena_raw_mem_alloc(...)                                                                    \
    ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 0))                                           \
    (ARENA_STATIC_ASSERT(false, "arena_raw_mem_alloc() must be called with at least one argument"), NULL) \
    (ARENA_PP_IF(ARENA_NAT_EQ(ARENA_ARGC(__VA_ARGS__), 1))                                          \
        (arena_raw_mem_alloc_default(__VA_ARGS__))                                                  \
        (arena_raw_mem_alloc_aligned(__VA_ARGS__)))
#endif

#if defined(ARENA_ON_UNIX) || defined(ARENA_ON_MACOS) || defined(ARENA_ON_ANDROID)
void arena_set_mmap_threshold(size_t size);
#endif
void arenalloc_init();
void arenalloc_deinit();

ARENA_END_DECLS

#endif // ARENALLOC_H