#include <stdio.h>
#include <stdlib.h>
#include "ceebee/cpu.h"
#include "ceebee/common.h"

//Provides codes to set term colors.
#include "ceebee/termColors.h"

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
  CPU cpu = initCPU();
  cpu.sp = 0x0000;
  cpu.pc = 0x0000; 
  #ifdef DEBUG
    printf(CYN "Cart size:\n\t%d\n" RESET, cartSize);
  #endif
 
  for (int i = 0; i < 2; i++)	{
    #ifndef DEBUG
      printDebug(cpu);
    #endif
    run_cycle(&cpu);
  }

  free((char *) cart);
  freeCPU(&cpu);
  #ifndef DEBUG
    printCard(EXITPATH);
  #endif
  return EXIT_SUCCESS;
}
