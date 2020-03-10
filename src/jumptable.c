#include <stdio.h>
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

void test_NOP() {
  printf("NOP\n");
}

void init_jmp (func_ptr jumptable[0xF][0xF]) {
  jumptable[0x00][0x00] = test_NOP;
  printf("howdy!");
}
