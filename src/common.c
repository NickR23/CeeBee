#include <stdio.h>
#include <stdlib.h>
#include "ceebee/termColors.h"
#include "ceebee/common.h"
#include "ceebee/cpu.h"

#define MEMDUMPLOC "./memorydump.txt"

void panic(void *cpu_ptr, char const *message) {
  fprintf(stderr, "%s\n", message); 
  #ifdef DEBUG
    printf("Dumping memory to %s\n", MEMDUMPLOC);
    FILE *fp = fopen(MEMDUMPLOC, "w");
    if (fp == NULL) {
      printf("Could not write memory dump to file!\n");
      exit(1);
    }
    CPU *cpu = (CPU*) cpu_ptr;
    for (int i = 0; i < 0x10000; i++) {
      fputc(cpu->mmu->ram[i], fp);
    }
    fclose(fp);
  #endif
  exit(1);
}
