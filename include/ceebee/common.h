#ifndef COMMON_H 
#define COMMON_H
  #define TITLEPATH "assets/title"
  #define EXITPATH "assets/exit"
  #include "ceebee/cpu.h"
  void dump_mem(CPU *cpu);
  void panic(void *cpu_ptr, char const *message);
#endif
