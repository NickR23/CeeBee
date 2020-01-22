#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "../lib/common.h"
#include "../lib/termColors.h"

unsigned char* loadCart(char const *cartPath) {
  FILE *fp;
  unsigned char *buffer;
  long cartLength;
  
  fp = fopen(cartPath, "rb");
  fseek(fp, 0, SEEK_END);
  cartLength = ftell(fp);
  rewind(fp);

  buffer = (unsigned char*)malloc(cartLength * sizeof(unsigned char));
  fread(buffer, cartLength, 1, fp);
  fclose(fp);
  return buffer;
}

void printCart(int start, unsigned char const *cart) {
  unsigned char const *ptr = &cart[start];
  for (int i = 0; i < 10; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}

void run_cycle(unsigned char const *cart) {
  printCart(0 ,cart);
}
