/* ============================================
* ARRAY EMULATION RUNTIME (array_runtime.h)
 * ============================================ */

// Array emulation runtime library
#ifndef ARRAY_RUNTIME_H
#define ARRAY_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dynamic array structure
typedef struct {
    void* data;
    int size;
    int capacity;
    int element_size;
} DynamicArray;

// Array runtime functions
DynamicArray* array_create(int element_size, int initial_capacity);
void array_destroy(DynamicArray* arr);
void* array_get(DynamicArray* arr, int index);
void array_set(DynamicArray* arr, int index, void* value);
void array_push(DynamicArray* arr, void* value);
void* array_pop(DynamicArray* arr);
void array_resize(DynamicArray* arr, int new_size);
int array_length(DynamicArray* arr);

// Bounds checking macros
#define ARRAY_BOUNDS_CHECK(arr, index) \
do { \
if ((index) < 0 || (index) >= (arr)->size) { \
fprintf(stderr, "Array index %d out of bounds [0, %d)\n", \
(index), (arr)->size); \
exit(1); \
} \
} while(0)

// Safe array access macros
#define ARRAY_GET_SAFE(arr, index, type) \
(*((type*)array_get((arr), (index))))

#define ARRAY_SET_SAFE(arr, index, value, type) \
do { \
type temp = (value); \
array_set((arr), (index), &temp); \
} while(0)

#endif // ARRAY_RUNTIME_H

/* ============================================
 * USAGE EXAMPLES
 * ============================================ */

/*
// Static array declaration
int numbers[10];
int matrix[3][4];

// Dynamic array declaration
int dynamic[] = dynamic;
float values[] = {1.0, 2.0, 3.0, 4.0};

// Array access
numbers[0] = 42;
int x = numbers[0];
matrix[1][2] = 100;

// Pointer arithmetic (array emulation)
int* ptr = &numbers[0];
*(ptr + 1) = 20; // Equivalent to numbers[1] = 20

// Array literals
int arr[] = {1, 2, 3, 4, 5};
*/