# Conservative Garbage Collector (C Implementation)

A minimal mark-and-sweep garbage collector demonstrating core memory management concepts.

## Core Architecture

### Data Structures

```c
// Memory block metadata
typedef struct Allocation {
    void* ptr;          // User-accessible memory
    size_t size;        // Allocated bytes
    char marked;        // GC mark bit
    void (*dtor)(void*); // Custom cleanup
    struct Allocation* next; // Linked list
} Allocation;

// Collector context
typedef struct {
    Allocation* allocations; // Allocation list
    void* stack_bottom;      // Stack reference
} GarbageCollector;

```

### Workflow Diagram

```
+---------------+       +---------------+       +---------------+
|  Allocation   |       |   Mark Phase  |       |  Sweep Phase  |
|  (gc_malloc)  |       +-------+-------+       +-------+-------+
+-------+-------+               |                       |
        |                       v                       v
        |               +-------+-------+       +-------+-------+
        |               | Stack Scan    |       | Free Unmarked |
        |               | Pointer Trace |       | Invoke DTORs  |
        |               +-------+-------+       +-------+-------+
        |                       |                       |
        +-----------------------+-----------------------+
                                |
                         +------+------+
                         | GC Context  |
                         | (Metadata)  |
                         +-------------+

```

## Implementation Details

### 1. Initialization (`gc_init`)

```c
void gc_init(GarbageCollector* gc, void* stack_bottom) {
    gc->allocations = NULL;
    gc->stack_bottom = stack_bottom;
}

```

Establishes:

- Empty allocation list
- Stack boundary reference

### 2. Memory Allocation (`gc_malloc`)

```c
void* gc_malloc(GarbageCollector* gc, size_t size, void (*dtor)(void*)) {
    Allocation* meta = malloc(sizeof(Allocation)); // 1. Allocate metadata
    void* user_mem = malloc(size);                 // 2. Allocate user memory

    meta->ptr = user_mem;                          // 3. Link components
    meta->marked = 0;
    meta->dtor = dtor;
    meta->next = gc->allocations;                  // 4. Add to list
    gc->allocations = meta;

    return user_mem;                               // 5. Return to user
}

```

### 3. Mark Phase (`gc_mark`)

```
Stack Scanning Process:
+------------------+
| Stack Frame      |
| +--------------+ |
| | Potential    +---> Check against
| | Pointers     | |    allocation list
| +--------------+ |
+------------------+
       |
       v
+------------------+
| Allocation List  |
| +---+  +---+     |
| |M1|->|M2|...   |
| +---+  +---+     |
+------------------+

```

### 4. Sweep Phase (`gc_sweep`)

```c
void gc_sweep(GarbageCollector* gc) {
    Allocation** curr = &gc->allocations;
    while (*curr) {
        Allocation* entry = *curr;
        if (!entry->marked) {
            // Free sequence
            if (entry->dtor) entry->dtor(entry->ptr);
            free(entry->ptr);
            *curr = entry->next;  // Bypass unmarked entry
            free(entry);
        } else {
            entry->marked = 0;    // Reset for next cycle
            curr = &entry->next;
        }
    }
}

```

## Key Workflows

### Collection Cycle (`gc_collect`)

```
1. Stack Scanning             2. Pointer Validation
+------------------+         +------------------+
| Scan stack       |         | Check 0xFE10     |
| addresses        |         | against          |
| 0xFE10, 0x1234   |         | allocation       |
+--------+---------+         | ranges           |
         |                   +--------+---------+
         v                            |
3. Recursive Marking                  v
+------------------+         +------------------+
| Mark Alloc1      |         | Ignore 0x1234    |
| Trace Alloc1     |         | (not in list)    |
| pointers         |         |                  |
+------------------+         +------------------+

```

### Memory Layout

```
+-----------------------+
| Allocation Metadata   |
| - ptr: 0x1000         |
| - size: 128           |
| - marked: 1           |
+----------+------------+
           |
           v
+-----------------------+
| User Memory (0x1000)  |
| Raw data/objects      |
+-----------------------+

```

## Critical Limitations

1. **Pointer Identification**
    
    ```
    +----------------+    +-----------------+
    | Stack Value    |    | Heap Allocation |
    | 0xFE10         +--->| 0x1000-0x10FF   |
    +----------------+    +-----------------+
    
    Potential false positive if:
    - Random value matches allocation range
    - Non-pointer data matches address pattern
    
    ```
    
2. **Performance Characteristics**
    
    ```
    | Operation     | Complexity  | Notes                     |
    |---------------|-------------|---------------------------|
    | Allocation    | O(1)        | List prepend              |
    | Marking       | O(S + N*D)  | S=stack size, D=ptr depth |
    | Sweeping      | O(N)        | N=allocations             |
    
    ```
    
3. **System Dependencies**
    - Relies on `__builtin_frame_address` (GCC/Clang)
    - Assumes contiguous stack growth
    - No thread safety mechanisms

## Performance Notes

| Operation | Complexity | Notes |
| --- | --- | --- |
| Allocation | O(1) | Linked-list append |
| Mark Phase | O(n + s) | n: allocations, s: stack size |
| Sweep Phase | O(n) | Linear traversal |
| Memory Overhead | 24-32 bytes/allocation | Header + alignment |
