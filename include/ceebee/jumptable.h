#ifndef JMPTBL_H
#define JMPTBL_H

typedef struct Opcode {
  // The number of cycles for the instruction
  int cycles;
  // The size of the instruction
  int size;
} Opcode;

#include "cpu.h"
void init_jmp(func_ptr jumptable[0xF][0xF]); 


#endif 
