#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "operations.h"
#include "../lib/common.h"
#include "../lib/termColors.h"

//Gets the next 16 bits in little endian from addr
//addr should be a pointer to the end of your op code.
int getNN(unsigned char const* cart, int16_t addr) {
  printf(GRN "Addr: %04x", addr);
  int16_t byte_2_addr = addr + 1;
  //Get first nibble
  char x2 = cart[byte_2_addr] & 0x0f;
  //Get second nibble
  char x1 = cart[byte_2_addr] >> 4;
  //Get third nibble
  char x4 = cart[addr] & 0x0f;
  //Get fourth nibble
  char x3 = cart[addr] >> 4;
  
  printf(RED "\t\tFirst nibble: %02x\n", x1);
  printf(RED "\t\tSecond nibble: %02x\n",x2); 
  printf(RED "\t\tThird nibble: %02x\n", x3);
  printf(RED "\t\tFourth nibble: %02x\n", x4);
  int nn = x1 << 12;
  nn = nn | (x2 << 8);
  nn = nn | (x3 << 4);
  nn = nn | x4;
  printf(GRN "NN: %x\n" RESET, nn); 

  return nn;
} 

unsigned char getByte(unsigned char const *cart, int16_t addr) {
  return cart[addr];
}

void printCpu(CPU cpu) {
  printf(MAG "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
  printf(YEL "I am a cpu!\nThis is how I feel\n" RESET);
  printf(MAG "\tStack pointer: 0x%04x\n" RESET, cpu.sp);
  printf(GRN "\tProgram counter: 0x%04x\n" RESET, cpu.pc);
  printf(MAG "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
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
  exec(op, cpu, cart);
}

void run_cycle(CPU *cpu, unsigned char const *cart, unsigned int const cartSize) {
  decodeOpCode(cpu, cart);
}
