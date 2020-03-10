#ifndef CPU_H
#define CPU_H

typedef void (*func_ptr)(void);

typedef struct CPU {
  //Regular registers
  //These are sometimes combined into 16-bit registers
  unsigned char b;
  unsigned char c;
  unsigned char d;
  unsigned char e;
  unsigned char a;
  unsigned char f; 
  unsigned char h;
  unsigned char l;
  //Program counter
  unsigned short pc;
  //Stack pointer
  unsigned short sp;
  //Memory
  unsigned char* ram;
  // Opcode jumptable
  func_ptr jumptable[0xF][0xF];
} CPU;

CPU initCPU();
void freeCPU(CPU *cpu);
unsigned short* getRP2Register(CPU *cpu, int index);
unsigned short* getRPRegister(CPU *cpu, int index);
unsigned char* getRegister(CPU *cpu, int index);
int getNN(unsigned char const *cart, unsigned short addr);
unsigned char getByte(unsigned char const *cart, unsigned short addr);
void printCpu(CPU cpu);
void run_cycle(CPU *cpu, unsigned char const *cart);
unsigned char* loadCart(char const *cartPath, unsigned int* cartSize);
#endif
