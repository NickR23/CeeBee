#ifndef JMPTBL_H
#define JMPTBL_H

#include "cpu.h"
void init_jmp(func_ptr jumptable[0xF][0xF]); 

#ifdef JMPTBL_TEST
void setCF(CPU *cpu, bool state);
void setHF(CPU *cpu, bool state);
void setZF(CPU *cpu, bool state);
void setNF(CPU *cpu, bool state);
void NOP(unsigned char const* cart, void *cpu, Op_info *info);
void LD_SP_d16(unsigned char const* cart, void *cpu, Op_info *info);
void LD_BC_d16(unsigned char const* cart, void *cpu, Op_info *info);
void LDINDR_BC_A(unsigned char const* cart, void *cpu, Op_info *info);
void INC_BC(unsigned char const* cart, void *cpu, Op_info *info);
void INC_B(unsigned char const* cart, void *cpu, Op_info *info);
void DEC_B(unsigned char const* cart, void *cpu, Op_info *info);
void LD_B_d8(unsigned char const* cart, void *cpu, Op_info *info);
void RLCA(unsigned char const* cart, void *cpu, Op_info *info);
void LD_a16_SP(unsigned char const* cart, void *cpu, Op_info *info);
void ADD_HL_BC(unsigned char const* cart, void *cpu, Op_info *info);
#endif

#endif 

