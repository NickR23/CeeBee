#define CPU_H
typedef struct CPU {
  //Regular registers
  //These are sometimes combined into 16-bit registers
  char a;
  char b;
  char c;
  char d;
  char e;
  char f; 
  char h;
  char l;
  //Program counter
  unsigned short pc;
  //Stack pointer
  unsigned short sp;
} CPU;
char* getRPRegister(CPU *cpu, int index);
char* getRegister(CPU *cpu, int index);
int getNN(unsigned char const *cart, int16_t addr);
unsigned char getByte(unsigned char const *cart, int16_t addr);
void printCpu(CPU cpu);
void run_cycle(CPU *cpu, unsigned char const *cart, unsigned int const cartSize);
unsigned char* loadCart(char const *cartPath, unsigned int* cartSize);
