#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "operations.h"
#include "../lib/common.h"
#include "../lib/termColors.h"

CPU initCPU()	{
	CPU cpu;
	cpu.ram = (unsigned char*) malloc(sizeof(unsigned char) * (65536));
	return cpu;
}

void freeCPU(CPU *cpu)	{
	free(cpu->ram);
}

unsigned short* getRP2Register(CPU *cpu, int index) {
  switch(index) {
    //For combo registers, I'm just returning the addr for the first register
		case 0:
      return (unsigned short*) &cpu->b;
			break;
		case 1:
      return (unsigned short*) &cpu->d;
			break;
		case 2:
			return (unsigned short*) &cpu->h;
			break;
		case 3:
			return (unsigned short*) &cpu->a;
		}
	return NULL;
}

unsigned short* getRPRegister(CPU *cpu, int index) {
  switch(index) {
    //For combo registers, I'm just returning the addr for the first register
		case 0:
      return (unsigned short*) &cpu->b;
			break;
		case 1:
      return (unsigned short*) &cpu->d;
			break;
		case 2:
			return (unsigned short*) &cpu->h;
			break;
		case 3:
			return &cpu->sp;
		}
	return NULL;
}

unsigned char* getRegister(CPU *cpu, int index) {
  switch(index){
    case 0:
	return &cpu->b;
    case 1:
	return &cpu->c;
    case 2:
	return &cpu->d;
    case 3:
	return &cpu->e;
    case 4:
	return &cpu->h;
    case 5:
	return &cpu->l;
    case 6:
	panic(RED "CPU doesn't have HL yet :(\n" RESET);
    case 7:
      return &cpu->a;
    default:
      return NULL;
    }
}
//Gets the next 16 bits in little endian from addr
//addr should be a pointer to the end of your op code.
int getNN(unsigned char const* cart, unsigned short addr) {
  int16_t byte_2_addr = addr + 1;
  //Get first nibble
  char x2 = cart[byte_2_addr] & 0x0f;
  //Get second nibble
  char x1 = cart[byte_2_addr] >> 4;
  //Get third nibble
  char x4 = cart[addr] & 0x0f;
  //Get fourth nibble
  char x3 = cart[addr] >> 4;
  
  int nn = x1 << 12;
  nn = nn | (x2 << 8);
  nn = nn | (x3 << 4);
  nn = nn | x4;

  return nn;
} 

unsigned char getByte(unsigned char const *cart, unsigned short addr) {
  return cart[addr];
}

void printCpu(CPU cpu) {
  printf(MAG "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
  printf(YEL "I am a cpu!\nThis is how I feel\n" RESET);
  printf(GRN "\t\tb: 0x%02x\n", cpu.b);
  printf(GRN "\t\tc: 0x%02x\n", cpu.c);
  printf(GRN "\t\td: 0x%02x\n", cpu.d);
  printf(GRN "\t\te: 0x%02x\n", cpu.e);
  printf(GRN "\t\ta: 0x%02x\n", cpu.a);
  printf(GRN "\t\tf: 0x%02x\n", cpu.f);
  printf(GRN "\t\th: 0x%02x\n", cpu.h);
  printf(GRN "\t\tl: 0x%02x\n", cpu.l);
  printf(GRN "\t\tpc: 0x%04x\n", cpu.pc);
  printf(GRN "\t\tsp: 0x%04x\n", cpu.sp);
  printf(MAG "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
}

unsigned char* loadCart(char const *cartPath, unsigned int* cartSize) {
  FILE *fp;
  unsigned char *buffer;
  long cartLength;
  
  fp = fopen(cartPath, "rb");
  if (fp == NULL) {
    panic(RED "Could not load cart" RESET);
  }
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

Opcode decodeOpCode(CPU *cpu, unsigned char const *cart) {
  unsigned char code = cart[cpu->pc];
  printf(RED "\tI found this: 0x%x\n" RESET, code);
  //See my notes for decoding explanation
  Opcode op;
  unsigned char x, y, z, p, q;
  op.x = code >> 6;
  unsigned char mask = 0x38;
  op.y = (code & mask) >> 3;
  op.p = op.y >> 1;
  op.q = op.y & 0x1;
  mask = 0x07;
  op.z = (code & mask);
	return op;
}

void run_cycle(CPU *cpu, unsigned char const *cart) {
	#ifdef DEBUG
		printf(YEL "PC: 0x%04hx\n" RESET, cpu->pc);
    printCpu(*cpu);
  #endif
  Opcode op = decodeOpCode(cpu, cart);
  exec(op, cpu, cart);
}
