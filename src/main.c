#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include "ceebee/cpu.h"
#include "ceebee/mmu.h"
#include "ceebee/common.h"

//Provides codes to set term colors.
#include "ceebee/termColors.h"

void usage() {
  printf(RED "Usage: gba \"CART_PATH\"\n" RESET);
  exit(EXIT_FAILURE);
}

bool continue_running = true;
void intHandler(int dummy) {
  continue_running = false;
}
  

//Prints one of my cool title cards
void printCard(char* messagePath) {
  FILE* fp = fopen(messagePath, "r");
  if (fp == NULL) {
    printf("Couldn't load my cool title graphic :(\n");
    return;
  }

  char cursor = fgetc(fp);
  while (cursor != EOF){
    printf(CYN "%c" RESET, cursor);
    cursor = fgetc(fp);
  }
  fclose(fp);
}


void dump_memory(CPU *cpu) {
  printf("Memory:\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  for (int i = 0; i < 0xFFFF; i++) {
    if (i % 10 == 0) printf("\n");
    printf("%02x ", cpu->mmu->ram[i]);
  }
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void run_emulator(CPU *cpu) {
  while (continue_running) {
    run_cycle(cpu);
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, intHandler);

  if (argc < 2)
    usage();
  char* cartPath = argv[1];
  #ifndef DEBUG
    printCard(TITLEPATH);
  #endif


  //Make CPU
  CPU cpu = initCPU();
  cpu.sp = 0x0000;
  cpu.pc = 0x0000; 

  //Load cart
  unsigned int cartSize = 0;
  loadCart(&cpu, cartPath, &cartSize);
  #ifdef DEBUG
    printf(CYN "Cart size:\n\t%d\n" RESET, cartSize);
  #endif
 
  run_emulator(&cpu);

  freeCPU(&cpu);
  printf(":))))\n");
  #ifndef DEBUG
    printCard(EXITPATH);
  #endif
  return EXIT_SUCCESS;
}
