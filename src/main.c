#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "operations.h"
#include "../lib/common.h"

//Provides codes to set term colors.
#include "../lib/termColors.h"

void usage() {
  printf(RED "Usage: gba \"CART_PATH\"\n" RESET);
  exit(EXIT_FAILURE);
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

int main(int argc, char** argv) {

  if (argc < 2)
    usage();
  char* cartPath = argv[1];
  #ifndef DEBUG
    printCard(TITLEPATH);
  #endif

  //Load cart
  unsigned int cartSize = 0;
  const unsigned char *cart = loadCart(cartPath, &cartSize);

  //Make CPU
  CPU cpu;
  cpu.sp = 0x0000;
  cpu.pc = 0x0000; 
  #ifdef DEBUG
    printf(CYN "Cart size:\n\t%d\n" RESET, cartSize);
  #endif

	LD16(getRPRegister(&cpu, 3), 0xc1);
  LD(getRegister(&cpu, 7), 0xab);
  printCpu(cpu);
  run_cycle(&cpu, cart);
  printCpu(cpu);
  run_cycle(&cpu, cart);
  printCpu(cpu);
  run_cycle(&cpu, cart);
  printCpu(cpu);

  free((char *) cart);
  #ifndef DEBUG
    printCard(EXITPATH);
  #endif
  return EXIT_SUCCESS;
}
