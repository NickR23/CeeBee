#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ceebee/termColors.h"
#include "ceebee/common.h"
#include "ceebee/cpu.h"
#include "ceebee/mmu.h"

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
  uint8_t code = readN(cpu, cpu->pc);
  
  printf("%s\n", message);
  printf("PC: 0x%04hx\tCode: 0x%02x\n", cpu->pc, code);
  printCpu(*cpu); 
  
  #ifdef DEBUG
    if (cpu_ptr != NULL) {
      dump_mem(cpu);
    }
  #endif
  exit(1);
}

#ifndef TESTING
void msleep(long msec) {
  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  int res; 
  do {
    res = nanosleep(&ts, &ts);
  } while(res);
}
#endif
