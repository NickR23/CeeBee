#include <stdio.h>
#include "ceebee/operations.h"
#include "ceebee/termColors.h"
#include "ceebee/common.h"

int add (int a, int b) {
  return a + b;
}

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

// Load 16 bits into destination
void indir16LD(unsigned short* dst, unsigned short* src) {
	*dst = *src;
}

// Load 8 bits into destination
void indirLD(unsigned char* dst, unsigned char* src) {
	*dst = *src;
}

// The indexing comes from the alu table here
// https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
void RALU(CPU *cpu, int index, unsigned char* rptr) {
  switch(index) {
    case 0:
      printf("Adding *0x%04x to into A\n", *rptr);
      cpu->a += *rptr; 
      break;
    case 1:
      printf("/tNOT IMPLEMENTED\nAdding with carry *0x%04x to into A\n", *rptr);
      break;
    case 2:
      printf("Subtracting *0x%04x to into A\n", *rptr);
      cpu->a -= *rptr;
      break;
    case 3:
      printf("Subtracting with carry *0x%04x to into A\n", *rptr);
      break;
     case 4:
      printf("And-ing *0x%04x and A\n", *rptr);
      cpu->a &= *rptr;
      break;
    case 5:
      printf("XOR-ing *0x%04x and A\n", *rptr);
      cpu->a ^= *rptr;
      break;
    case 6:
      printf("OR-ing *0x%04x and A\n", *rptr);
      cpu->a |= *rptr;
      break;
    case 7:
      printf("CP-ing *0x%04x into A\n", *rptr);
      break;
  }
}
  
void LD16(unsigned short* rptr, unsigned short data) {
  *rptr = data;
}

void LD(unsigned char* rptr, unsigned char data) {
  *rptr = data;
} 

void exec(Opcode op, CPU *cpu,  unsigned char const *cart) {

  #ifdef DEBUG
    printOp(op);
  #endif
  
  void *func = cpu->jumptable[0][0];

}
