#include <stdio.h>
#include "operations.h"
#include "../lib/termColors.h"
#include "../lib/common.h"
const char r[] = {B, C, D, E, H, L, HL, A};
const char rp[] = {BC, DE, HL, SP};
const char rp2[] = {BC, DE, HL, AF};

void printOp(Opcode op){
  printf("\t\tx: %02x\n", op.x);
  printf("\t\ty: %02x\n", op.y);
  printf("\t\tz: %02x\n", op.z);
  printf("\t\tp: %02x\n", op.p);
  printf("\t\tq: %02x\n", op.q);
}

void NOP() {
  printf("\tOperation: NOP");
}

void ld(unsigned char reg, int16_t value) {
  printf("Loading %04x into %04x\n", value, reg);
}

void exec(Opcode op, CPU *cpu,  unsigned char const *cart) {
  printOp(op);
  if (op.x == 0){
    if (op.z == 0) {
      if (op.y == 0) {
        NOP();
        return;
      }
    }
    if (op.z == 1) {
      if (op.q == 0){
         printf(CYN "Loading addr: %x into sp\n"RESET, getNN(cart, cpu->pc + 1)); 
         cpu->pc += 3;
         return;
      }
    }
  }
  panic("I don't know that opcode yet :(");
}
