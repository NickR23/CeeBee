#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "operations.h"
#include "../lib/common.h"
#include "../lib/termColors.h"

unsigned char getByte(unsigned char const *cart) {
  return cart[0];
}

void printCpu(CPU cpu) {
  printf(YEL "I am a cpu!\nThis is how I feel\n" RESET);
  printf(MAG "\tStack pointer: 0x%04x\n" RESET, cpu.sp);
  printf(GRN "\tProgram counter: 0x%04x\n" RESET, cpu.pc);
}

unsigned char* loadCart(char const *cartPath, unsigned int* cartSize) {
  FILE *fp;
  unsigned char *buffer;
  long cartLength;
  
  fp = fopen(cartPath, "rb");
  fseek(fp, 0, SEEK_END);
  cartLength = ftell(fp);
  *cartSize = cartLength;
  rewind(fp);

  buffer = (unsigned char*)malloc(cartLength * sizeof(unsigned char));
  fread(buffer, cartLength, 1, fp);
  fclose(fp);
  return buffer;
}

void printCart(int start, unsigned char const *cart) {
  unsigned char const *ptr = &cart[start];
  for (int i = 0; i < 20; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}

void decodeOpCode(CPU *cpu, unsigned char const *cart) {
  printf(CYN "\n\nI am here: 0x%04x\n" RESET , cpu->pc);  
  unsigned char code = cart[cpu->pc];
  printf(RED "\tI found this: 0x%x\n" RESET, code);
  //See my notes for decoding explanation
  Opcode op;
  unsigned char x, y, z, p, q;
  op.x = x >> 5;
  unsigned char mask = 0x38;
  op.y = (code & mask) >> 3;
  op.p = op.y >> 1;
  op.q = op.y & 0x1;
  mask = 0x07;
  op.z = (code & mask);
  
  printf("\t\tx: %02x\n", op.x);
  printf("\t\ty: %02x\n", op.y);
  printf("\t\tz: %02x\n", op.z);
  printf("\t\tp: %02x\n", op.p);
  printf("\t\tq: %02x\n", op.q);
}

void run_cycle(CPU *cpu, unsigned char const *cart, unsigned int const cartSize) {
  decodeOpCode(cpu, cart);
  cpu->pc++;
}
