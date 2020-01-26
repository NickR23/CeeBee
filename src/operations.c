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

void RXOR(CPU *cpu, char reg) {
  if (reg == 0) cpu->a = cpu->a ^ cpu ->a;
}

void CPL(CPU *cpu) {
  cpu->a = cpu->a ^ cpu->a;
}

void ld(unsigned char reg, int16_t value) {
  printf("Loading %04x into %04x\n", value, reg);
}

void exec(Opcode op, CPU *cpu,  unsigned char const *cart) {

  #ifdef DEBUG
    printOp(op);
  #endif

  if (op.x == 0){
    
    if (op.z == 0) {
      if (op.y == 0) {
        NOP();
        return;
      }
      else if (op.y == 1) {
        #ifdef DEBUG
           printf(CYN "Loading sp into addr: %x\n"RESET, getNN(cart, cpu->pc + 1)); 
        #endif
        cpu->pc += 3;
				return;
      }
      else if (op.y == 2) {
        #ifdef DEBUG
           printf(CYN "STOPPING\n" RESET); 
        #endif
        cpu->pc += 1;
				return;
      }
      else if (op.y == 3) {
				#ifdef DEBUG
          printf(CYN "Conditional jump by displacement byte\n" RESET);
        #endif
        cpu->pc  += 2;
				return;
      }
      else if (op.y >= 4 && op.y <= 7) {
        #ifdef DEBUG
          printf(CYN "Conditional jump based on condition\n" RESET);
        #endif 
        cpu->pc += 2;
				return;
      }
    }
    if (op.z == 1) {
      if (op.q == 0) {
         #ifdef DEBUG
           printf(CYN "Loading addr: %x into sp\n"RESET, getNN(cart, cpu->pc + 1)); 
         #endif
         cpu->pc += 3;
         return;
      }
    }
    else if (op.z == 7) { 
      if (op.y == 5) {
        CPL(cpu);
        return;
      }
    }

  }
  else if (op.x == 1) {
    // This op replaces the LD HL, HL op.
    if (op.z == 6 && op.y == 6) {
      printf(RED "HALTING\n" RESET);
      cpu->pc += 1;
      return;
    }
    else {
      printf(CYN "Loading 0x%04x into 0x%04x\n", r[op.z], r[op.y]);
			return;
    }
  }
  else if (op.x == 2) {
    RXOR(cpu, r[op.z]);
    cpu->pc += 1;
    return;
  }
  printf("I don't know that opcode yet :(\n");
}
