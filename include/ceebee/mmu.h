#ifndef MMU_H
#define MMU_H
#include <stdlib.h> 
#include "ceebee/cpu.h"
void mmu_load_boot_rom(MMU *mmu);
void writeNN(CPU *cpu, uint16_t addr, uint16_t val);
uint16_t readNN(CPU *cpu, uint16_t addr);
uint8_t readN(CPU *cpu, uint16_t addr);
void writeN(CPU *cpu, uint16_t addr, uint8_t val);
void loadCart(CPU *cpu, char const *cartPath, unsigned int* cartSize);
#endif
