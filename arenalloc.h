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

#include <stddef.h>
#include "tinycthread/source/tinycthread.h"

#if defined(MAX_DTOR_COUNT)
    #undef MAX_DTOR_COUNT
#endif
#define MAX_DTOR_COUNT 32

#if !defined(MAX_ARENA_COUNT)
    #define MAX_ARENA_COUNT 20
#endif

#if defined(create_region)
    #undef create_region
#endif
#define create_region(arena, size, type) create_region_aligned(arena, size, _Alignof(type))

typedef struct arena_region
{
    size_t padding_left;
    size_t padding_right;
    size_t start_pos;
    size_t end_pos;
    size_t alignment;
} arena_region;

typedef struct arena 
{
    size_t region_count;
    void* memory_start;
    void* memory_end;
    arena_region *first_region;
    arena_region *last_region;
    mtx_t lock;
}arena;

#endif // ARENALLOC_H
