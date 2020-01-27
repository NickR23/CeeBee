#ifndef CPU_H 
  #include "cpu.h"
#endif

#define OPERATIONS_H

//Register byte abstractions
#define A (char)0x00000000
#define B (char)0x00000001
#define C (char)0x00000010
#define D (char)0x00000011
#define E (char)0x00000100
#define H (char)0x00000101
#define L (char)0x00000110
#define BC (char)0x00000111
#define DE (char)0x00001000 
#define HL (char)0x00001001
#define SP (char)0x00001010
#define AF (char)0x00001011

typedef struct Opcode {
  unsigned char x;
  unsigned char y;
  unsigned char z;
  unsigned char p;
  unsigned char q;
} Opcode;

extern const char r[];
void LD16(unsigned short* rptr, short data);
void LD(unsigned char* rptr, char data);
void exec(Opcode op, CPU *cpu, unsigned char const *cart);
