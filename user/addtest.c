#include "kernel/types.h"
#include "user/user.h"

int main() {
    int a = 11;
    int b = 22;

    int res = adder(a, b);

    printf("%d + %d = %d\n", a, b, res);
    return 0;
}