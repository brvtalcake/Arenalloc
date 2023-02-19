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
#include <stdlib.h>
#include <string.h>

#define arenalock(arena) mtx_lock(&arena->lock)
#define arena_unlock(arena) mtx_unlock(&arena->lock)

static arena* arena_pool[MAX_ARENA_COUNT] = {NULL};

arena* arenalloc(size_t size)
{
    arena* arena = malloc(sizeof(arena));
    if (arena == NULL)
    {
        return NULL;
    }
    arena->memory_start = malloc(size);
    if (arena->memory_start == NULL)
    {
        free(arena);
        return NULL;
    }
    arena->memory_end = arena->memory_start + size;
    arena->region_count = 0;
    arena->first_region = NULL;
    arena->last_region = NULL;
    mtx_init(&arena->lock, mtx_plain);
    if (add_arena_to_pool(arena) != 0)
    {
        free(arena->memory_start);
        free(arena);
        return NULL;
    }
    
    return arena;
}

static int add_arena_to_pool(arena* arena)
{
    size_t first_pool = 0;
    while (arena_pool[first_pool] != NULL && first_pool < MAX_ARENA_COUNT)
    {
        first_pool++;
    }
    if (first_pool >= MAX_ARENA_COUNT)
    {
        return -1;
    }
    arena_pool[first_pool] = arena;
    return 0;
}

static arena* arenapool_get(size_t index)
{
    if (index >= MAX_ARENA_COUNT)
    {
        return NULL;
    }
    return arena_pool[index];
}

arena* create_region_aligned(arena* arena, size_t size, size_t alignment)
{
    arenalock(arena);
    
}