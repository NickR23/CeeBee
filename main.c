#include <stdio.h>
#include <stdlib.h>
#include "lib/common.h"
//Provides codes to set term colors.
//example:
//  printf(RED "red text" RESET);
#include "lib/termColors.h"

void usage() {
  printf(RED "Usage: gba \"CART_PATH\"\n" RESET);
  exit(EXIT_FAILURE);
}

//Reads the title
void printTitle() {
  FILE* fp = fopen(TITLEPATH, "r");
  char cursor = fgetc(fp);
  while (cursor != EOF){
    printf(CYN "%c" RESET, cursor);
    cursor = fgetc(fp);
  } 
}

int main(int argc, char** argv) {
  if (argc < 2)
    usage();
  char* cartPath = argv[1];
  printf("Reading cart: %s\n", cartPath);
  printTitle();

  return 0;
}
