#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "../lib/common.h"
#include "../lib/termColors.h"

unsigned char* loadCart(char const *cartPath, unsigned int* cartSize) {
  FILE *fp;
  unsigned char *buffer;
  long cartLength;
  
  fp = fopen(cartPath, "rb");
  fseek(fp, 0, SEEK_END);
  cartLength = ftell(fp);
  *cartSize = cartLength;
  rewind(fp);

  buffer = (unsigned char*)malloc(cartLength * sizeof(unsigned char));
  fread(buffer, cartLength, 1, fp);
  fclose(fp);
  return buffer;
}

void printCart(int start, unsigned char const *cart) {
  unsigned char const *ptr = &cart[start];
  for (int i = 0; i < 20; i++) {
    printf(GRN "%02x " RESET, cart[i]);
  }
  printf("\n");
}


unsigned char getByte(unsigned char const *cart) {
  return cart[0];
}

void run_cycle(unsigned char const *cart, unsigned int const cartSize) {
  printCart(0 ,cart);
}
