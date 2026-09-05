#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_adder(void)
{
    int a,b;
    argint(0, &a);
    argint(0, &b);
    printk("adder got two numbers: %d and %d.\n", a, b);
    return a + b;
}