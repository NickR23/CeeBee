#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void usage() {
  printf("Usage: gba \"CART_PATH\"\n");
  exit(EXIT_FAILURE);
}

//Reads the title
void printTitle() {
  FILE* fp = fopen(TITLEPATH, "r");
  char cursor = fgetc(fp);
  while (cursor != EOF){
    printf("%c", cursor);
    cursor = fgetc(fp);
  } 
}

int main(int argc, char** argv) {
  if (argc < 2)
    usage();

  printTitle();

  return 0;
}
