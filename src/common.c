#include <stdio.h>
#include <stdlib.h>
#include "ceebee/termColors.h"
#include "ceebee/common.h"
#include "ceebee/cpu.h"

#define MEMDUMPLOC "./memorydump.txt"

void dump_mem(CPU *cpu) {
    printf("Dumping memory to %s\n", MEMDUMPLOC);
    FILE *fp = fopen(MEMDUMPLOC, "w");
    if (fp == NULL) {
      printf("Could not write memory dump to file!\n");
      exit(1);
    }
    for (int i = 0; i < 0x10000; i++) {
      fputc(cpu->mmu->ram[i], fp);
    }
    fclose(fp);
}

void panic(void *cpu_ptr, char const *message) {
  CPU *cpu = (CPU *) cpu_ptr;
  fprintf(stderr, "%s ** pc 0x%04x\n", message, cpu->pc); 
  #ifdef DEBUG
    if (cpu_ptr != NULL) {
      dump_mem(cpu);
    }
  #endif
  exit(1);
}
