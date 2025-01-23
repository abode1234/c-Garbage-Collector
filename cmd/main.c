#include "../include/gc.h"
#include <stdio.h>

void custom_destructor(void* ptr) {
    printf("destructor: %p\n", ptr);
}

typedef struct Node {
    int data;
    struct Node* next;
} Node;

int main(int argc, char* argv[]) {
    GarbageCollector gc;
    gc_init(&gc, &argc);

    Node* n1 = gc_malloc(&gc, sizeof(Node), custom_destructor);
    Node* n2 = gc_malloc(&gc, sizeof(Node), NULL);

    n1->data = 10;
    n2->data = 20;
    n1->next = n2;

    printf("n1 = %p\n", n1->next);
    printf("before = %zu\n", gc_count_allocations(&gc));

    n1->next = NULL;

    gc_collect(&gc);

    printf("after = %zu\n", gc_count_allocations(&gc));

    return 0;
}
