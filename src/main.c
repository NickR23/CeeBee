#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "../lib/common.h"

//Provides codes to set term colors.
//example:
//  printf(RED "red text" RESET);
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
  printCard(TITLEPATH);
  //Load cart
  const unsigned char *cart = loadCart(cartPath);
  run_cycle(cart);

  free((char *) cart);
  printCard(EXITPATH);
  return 0;
}
