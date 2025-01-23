#ifndef GC_H
#define GC_H

#include <stdlib.h>
#include <stddef.h>

typedef struct Allocation {
    void* ptr;
    size_t size;
    char marked;
    void (*dtor)(void*);
    struct Allocation* next;
} Allocation;

typedef struct {
    Allocation* allocations;
    void* stack_bottom;
} GarbageCollector;

void gc_init(GarbageCollector* gc, void* stack_bottom);
void* gc_malloc(GarbageCollector* gc, size_t size, void (*dtor)(void*));
void gc_collect(GarbageCollector* gc);
size_t gc_count_allocations(const GarbageCollector* gc);

#endif
