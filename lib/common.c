#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void panic(char const *message) {
  fprintf(stderr, "%s\n", message); 
  exit(1);
}
