#define CPU_H
typedef struct CPU {
  //Regular registers
  //These are sometimes combined into 16-bit registers
  int16_t a;
  int16_t b;
  int16_t c;
  int16_t d;
  int16_t e;
  int16_t f; 
  int16_t h;
  int16_t l;
  //Program counter
  int16_t pc;
  //Stack pointer
  int16_t sp;
} CPU;

int getNN(unsigned char const *cart, int16_t addr);
unsigned char getByte(unsigned char const *cart, int16_t addr);
void printCpu(CPU cpu);
void run_cycle(CPU *cpu, unsigned char const *cart, unsigned int const cartSize);
unsigned char* loadCart(char const *cartPath, unsigned int* cartSize);
