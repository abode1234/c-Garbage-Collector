# Conservative Garbage Collector Implementation

A lightweight, conservative mark-and-sweep garbage collector implementation in C, designed for automatic memory management in C programs.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Building and Installation](#building-and-installation)
- [Technical Architecture](#technical-architecture)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Advanced Topics](#advanced-topics)
- [Performance Analysis](#performance-analysis)
- [Limitations and Considerations](#limitations-and-considerations)
- [Contributing](#contributing)
- [License](#license)

## Overview

This garbage collector implements a conservative mark-and-sweep algorithm that automatically manages memory in C programs. It tracks allocations, identifies unreachable memory, and frees resources when they're no longer needed.

### Key Features
- Conservative stack scanning
- Automatic memory reclamation
- Custom destructors support
- Zero configuration required
- Minimal runtime overhead
- No external dependencies

## Building and Installation

### Prerequisites
- GCC or Clang compiler
- Make build system
- Standard C library

### Build Steps
```bash
# Clone the repository
git clone https://github.com/yourusername/garbage-collector.git
cd garbage-collector

# Build the library
make

# Run tests
make test

# Install (optional)
sudo make install
```

### Integration
Add the following to your project:
```c
#include <gc.h>
```

Link with:
```bash
gcc your_program.c -lgc
```

## Technical Architecture

### Memory Management Model

The garbage collector uses a three-phase approach:
1. **Allocation Tracking**
   ```c
   typedef struct Allocation {
       void* ptr;              // Allocated memory pointer
       size_t size;           // Allocation size
       char marked;           // Mark bit for GC
       void (*dtor)(void*);   // Custom destructor
       struct Allocation* next; // Next in allocation list
   } Allocation;
   ```

2. **Mark Phase**
   - Stack scanning using pointer analysis
   - Recursive marking of reachable objects
   - Conservative pointer identification

3. **Sweep Phase**
   - Cleanup of unreachable objects
   - Destructor invocation
   - Memory deallocation

### Memory Layout
```
Stack Memory
+----------------+
|  Stack Bottom  |
|    ↓          |
|    Stack      |
|    Frame      |
|    ↓          |
|  Stack Top    |
+----------------+

Heap Memory
+----------------+
| Allocation 1   |
| - Header      |
| - User Data   |
+----------------+
| Allocation 2   |
| - Header      |
| - User Data   |
+----------------+
```

## API Reference

### Core Functions

#### Initialization
```c
void gc_init(GarbageCollector* gc, void* stack_bottom);
```
Parameters:
- `gc`: Pointer to GarbageCollector instance
- `stack_bottom`: Stack bottom reference point

#### Memory Allocation
```c
void* gc_malloc(GarbageCollector* gc, size_t size, void (*dtor)(void*));
```
Parameters:
- `gc`: GarbageCollector instance
- `size`: Requested allocation size
- `dtor`: Optional destructor function

Returns:
- Pointer to allocated memory

#### Garbage Collection
```c
void gc_collect(GarbageCollector* gc);
```
Triggers immediate garbage collection cycle

#### Allocation Count
```c
size_t gc_count_allocations(const GarbageCollector* gc);
```
Returns current number of tracked allocations

## Usage Examples

### Basic Usage
```c
#include <gc.h>

int main(int argc, char* argv[]) {
    GarbageCollector gc;
    gc_init(&gc, &argc);

    // Simple allocation
    int* numbers = gc_malloc(&gc, sizeof(int) * 100, NULL);

    // Work with memory normally
    for (int i = 0; i < 100; i++) {
        numbers[i] = i;
    }

    // GC will clean up automatically
    return 0;
}
```

### Complex Data Structures
```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;

void node_destructor(void* ptr) {
    Node* node = (Node*)ptr;
    printf("Destroying node with data: %d\n", node->data);
}

int main() {
    GarbageCollector gc;
    gc_init(&gc, &argc);

    // Create linked list
    Node* head = gc_malloc(&gc, sizeof(Node), node_destructor);
    head->data = 1;
    head->next = gc_malloc(&gc, sizeof(Node), node_destructor);
    head->next->data = 2;
    head->next->next = NULL;

    // Memory is automatically managed
    gc_collect(&gc);
}
```

### Resource Management
```c
typedef struct {
    FILE* file;
    char* buffer;
} ResourceHandle;

void resource_cleanup(void* ptr) {
    ResourceHandle* handle = (ResourceHandle*)ptr;
    if (handle->file) fclose(handle->file);
    // Buffer will be freed automatically
}

void process_file(GarbageCollector* gc, const char* filename) {
    ResourceHandle* handle = gc_malloc(gc, sizeof(ResourceHandle), resource_cleanup);
    handle->file = fopen(filename, "r");
    handle->buffer = gc_malloc(gc, 1024, NULL);

    // Use resources...

    // No need to manually clean up
}
```

## Advanced Topics

### Memory Layout Optimization
- Allocation grouping for locality
- Pointer alignment considerations
- Cache-friendly data structures

### Collection Strategies
1. **Frequency Control**
   - Collection triggers
   - Memory pressure handling
   - Automatic threshold adjustment

2. **Root Set Management**
   - Stack scanning optimization
   - Register handling
   - Conservative pointer identification

### Thread Safety Considerations
- Current implementation is single-threaded
- Mutex integration points
- Thread-local allocation tracking

## Performance Analysis

### Time Complexity
- Allocation: O(1)
- Collection: O(s + n)
  - s: Stack size
  - n: Number of allocations

### Space Overhead
- 16-24 bytes per allocation (64-bit system)
- Linear scaling with allocation count

### Optimization Tips
1. **Reduce Collection Frequency**
   - Batch operations
   - Memory pooling
   - Strategic collection points

2. **Minimize False Positives**
   - Align data structures
   - Clear unused memory
   - Avoid pointer-like values

## Limitations and Considerations

### Current Limitations
1. **Conservative Nature**
   - False positives in pointer detection
   - Memory retention issues
   - Stack value interference

2. **Performance Impact**
   - Full stack scanning overhead
   - Non-compacting collection
   - Sequential allocation tracking

3. **Feature Gaps**
   - No generational collection
   - Limited root set scanning
   - No concurrent collection

### Best Practices
1. **Memory Management**
   - Group related allocations
   - Use appropriate collection points
   - Monitor allocation patterns

2. **Resource Handling**
   - Implement proper destructors
   - Handle circular references
   - Clear unused pointers

3. **Performance Tuning**
   - Profile collection frequency
   - Monitor memory pressure
   - Optimize data structures

## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License.

---
