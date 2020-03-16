#ifndef JMPTBL_H
#define JMPTBL_H

#include "cpu.h"
void init_jmp(func_ptr jumptable[0xF][0xF]); 

#ifdef JMPTBL_TEST
void setCF(CPU *cpu, bool state);
void setHF(CPU *cpu, bool state);
void setZF(CPU *cpu, bool state);
void setNF(CPU *cpu, bool state);
void NOP(void *cpu, Op_info *info);
void LD_BC_d16(void *cpu, Op_info *info);
void LDINDR_BC_A(void *cpu, Op_info *info);
void INC_BC(void *cpu, Op_info *info);
void INC_B(void *cpu, Op_info *info);
void DEC_B(void *cpu, Op_info *info);
void LD_B_d8(void *cpu, Op_info *info);
void RLCA(void *cpu, Op_info *info);
void LD_a16_SP(void *cpu, Op_info *info);
void ADD_HL_BC(void *cpu, Op_info *info);
void LD_A_INDIR_BC(void *cpu, Op_info *info);
void DEC_BC(void *cpu, Op_info *info);
void INC_C(void *cpu, Op_info *info);
void DEC_C(void *cpu, Op_info *info);
void LD_C_d8(void *cpu, Op_info *info);
void RRCA(void *cpu, Op_info *info);
void RLA(void *cpu, Op_info *info);
void LD_DE_d16(void *cpu, Op_info *info);
void LD_HL_d16(void *cpu, Op_info *info);
void LD_SP_d16(void *cpu, Op_info *info);
void LDINDR_DE_A(void *cpu, Op_info *info);
void LDINC_HL_A(void *cpu, Op_info *info);
void LDDEC_HL_A(void *cpu, Op_info *info);
void INC_DE(void *cpu, Op_info *info);
void INC_HL(void *cpu, Op_info *info);
void INC_SP(void *cpu, Op_info *info);
void INC_D(void *cpu, Op_info *info);
void INC_H(void *cpu, Op_info *info);
void INCINDR_HL(void *cpu, Op_info *info);
void DEC_D(void *cpu, Op_info *info);
void DEC_H(void *cpu, Op_info *info);
void DECINDR_HL(void *cpu, Op_info *info);
void LD_D_d8(void *cpu, Op_info *info);
void LD_H_d8(void *cpu, Op_info *info);
void LDINDR_HL_d8(void *cpu, Op_info *info);
void SCF(void *cpu, Op_info *info);
void JR_r8(void *cpu, Op_info *info);
void JR_Z_r8(void *cpu, Op_info *info);
void JR_C_r8(void *cpu, Op_info *info);
void ADD_HL_DE(void *cpu, Op_info *info);
void ADD_HL_HL(void *cpu, Op_info *info);
void ADD_HL_SP(void *cpu, Op_info *info);
#endif

#endif 

