#ifndef JMPTBL_H
#define JMPTBL_H

#include "cpu.h"
void init_jmp(func_ptr jumptable[0xF][0xF]); 

#ifdef JMPTBL_TEST
void setCF(CPU *cpu, bool state);
void setHF(CPU *cpu, bool state);
#endif

#endif 

