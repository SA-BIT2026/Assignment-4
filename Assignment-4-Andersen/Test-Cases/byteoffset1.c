#include <stdlib.h>

typedef struct StructA
{
    int foo;
    int (*f)(void);
} StructA;

__attribute__((noinline))
int FuncA() {
    return 1;
}

__attribute__((noinline))
int CallF(StructA *structA) {
    int ret = structA->f();
    return ret;
}

int main() {
    StructA *structA = malloc(sizeof(StructA));
    structA->f = FuncA;

    int ret = CallF(structA);
    return ret;
}
