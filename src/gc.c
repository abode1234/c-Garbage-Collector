#include "../include/gc.h"
#include <assert.h>
#include <stdio.h>

static void add_allocation(GarbageCollector* gc, Allocation* alloc) {
    alloc->next = gc->allocations;
    gc->allocations = alloc;
}

void gc_init(GarbageCollector* gc, void* stack_bottom) {
    gc->allocations = NULL;
    gc->stack_bottom = stack_bottom;
}

void* gc_malloc(GarbageCollector* gc, size_t size, void (*dtor)(void*)) {
    Allocation* alloc = malloc(sizeof(Allocation));
    assert(alloc && "Failed to allocate Allocation");

    alloc->ptr = malloc(size);
    assert(alloc->ptr && "Failed to allocate memory");

    alloc->size = size;
    alloc->marked = 0;
    alloc->dtor = dtor;
    add_allocation(gc, alloc);

    return alloc->ptr;
}

static void gc_mark(GarbageCollector* gc, void* ptr) {
    for (Allocation* a = gc->allocations; a; a = a->next) {
        if (a->ptr == ptr && !a->marked) {
            a->marked = 1;
            for (char* p = a->ptr; p < (char*)a->ptr + a->size; p += sizeof(void*)) {
                void* child = *(void**)p;
                gc_mark(gc, child);
            }
        }
    }
}

static void gc_sweep(GarbageCollector* gc) {
    Allocation** curr = &gc->allocations;
    while (*curr) {
        Allocation* entry = *curr;
        if (!entry->marked) {
            if (entry->dtor) entry->dtor(entry->ptr);
            free(entry->ptr);
            *curr = entry->next;
            free(entry);
        } else {
            entry->marked = 0;
            curr = &entry->next;
        }
    }
}

size_t gc_count_allocations(const GarbageCollector* gc) {
    size_t count = 0;
    Allocation* current = gc->allocations;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}


void gc_collect(GarbageCollector* gc) {
    void* stack_top = __builtin_frame_address(0);
    for (void** p = (void**)stack_top; p < (void**)gc->stack_bottom; p++) {
        void* ptr = *p;
        gc_mark(gc, ptr);
    }
    gc_sweep(gc);
}
