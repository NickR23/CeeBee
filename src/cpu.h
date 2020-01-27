#define CPU_H
typedef struct CPU {
  //Regular registers
  //These are sometimes combined into 16-bit registers
  unsigned char a;
  unsigned char b;
  unsigned char c;
  unsigned char d;
  unsigned char e;
  unsigned char f; 
  unsigned char h;
  unsigned char l;
  //Program counter
  unsigned short pc;
  //Stack pointer
  unsigned short sp;
} CPU;
unsigned short* getRPRegister(CPU *cpu, int index);
unsigned char* getRegister(CPU *cpu, int index);
int getNN(unsigned char const *cart, int16_t addr);
unsigned char getByte(unsigned char const *cart, int16_t addr);
void printCpu(CPU cpu);
void run_cycle(CPU *cpu, unsigned char const *cart, unsigned int const cartSize);
unsigned char* loadCart(char const *cartPath, unsigned int* cartSize);
