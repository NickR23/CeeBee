#include <stdio.h>
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"
#include "ceebee/termColors.h"

void test_NOP() {
  printf("NOP\n");
}

void test_LD() {
  printf(CYN "\nI am at 0x31!\n" RESET);
}

void init_jmp (func_ptr jumptable[0xF][0xF]) {
  jumptable[0x00][0x00] = test_NOP;
  jumptable[0x03][0x01] = test_LD;
}
