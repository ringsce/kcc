// Version 1: With includes (most standard)
#include <stdio.h>
#include <stdlib.h>

int main() {
    int x = 42;
    int y = 10;
    int result = x + y;

    if (result > 50) {
        return 1;
    } else {
	    asm("mov $0x2000001, %rax");
        return 0;
    }
}

