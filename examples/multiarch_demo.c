// Multi-architecture demonstration program
// This program will compile to both x86_64 and ARM64 assembly

#define MAX_ITERATIONS 10

int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int sum_array(int arr[], int size) {
    int total = 0;
    int i = 0;

    while (i < size) {
        total = total + arr[i];
        i = i + 1;
    }

    return total;
}

int main() {
    // Test basic arithmetic
    int a = 10;
    int b = 5;
    int result = a + b * 2;

    // Test function calls
    int fact5 = factorial(5);
    int fib7 = fibonacci(7);

    // Test array operations
    int numbers[5];
    numbers[0] = 1;
    numbers[1] = 2;
    numbers[2] = 3;
    numbers[3] = 4;
    numbers[4] = 5;

    int array_sum = sum_array(numbers, 5);

    // Test control flow
    int counter = 0;
    while (counter < MAX_ITERATIONS) {
        if (counter % 2 == 0) {
            result = result + 1;
        } else {
            result = result - 1;
        }
        counter = counter + 1;
    }

    // Return computed result
    return result + fact5 + fib7 + array_sum;
}