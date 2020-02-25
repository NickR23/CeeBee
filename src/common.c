#include <stdio.h>
#include <stdlib.h>
#include "ceebee/termColors.h"
#include "ceebee/common.h"

void panic(char const *message) {
  fprintf(stderr, "%s\n", message); 
  exit(1);
}

void printDebug(CPU cpu)  {
	printf(RED "---CPU-----------------------------");
}
