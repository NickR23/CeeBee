#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "ceebee/jumptable_test.h"
#include "ceebee/jumptable.h"
#include "ceebee/cpu.h"

void test_initCPU(void ** state) {
  CPU cpu = initCPU();
  assert_true(cpu.af == 0x0000);
  assert_true(cpu.bc == 0x0000);
  assert_true(cpu.de == 0x0000);
  assert_true(cpu.hl == 0x0000);
  assert_true(cpu.sp == 0x0000);
  assert_true(cpu.pc == 0x0000);

  freeCPU(&cpu);
}

void test_getRegister16(void ** state) {
  CPU cpu = initCPU();
  
  cpu.af = 0x1234;
  cpu.bc = 0x8765;
  cpu.de = 0x5FA1;
  cpu.hl = 0x7DD1;
  cpu.sp = 0x5766;
  cpu.pc = 0xABCD;
  assert_true(*getRegister16(&cpu,A) == 0x1234);
  assert_true(*getRegister16(&cpu,B) == 0x8765);
  assert_true(*getRegister16(&cpu,D) == 0x5FA1);
  assert_true(*getRegister16(&cpu,H) == 0x7DD1);
  assert_true(*getRegister16(&cpu,SP) == 0x5766);
  assert_true(*getRegister16(&cpu,PC) == 0xABCD);
  assert_null(getRegister16(&cpu,99));

  freeCPU(&cpu);
}

void test_getRegister(void ** state) {
  CPU cpu = initCPU();
  
  cpu.af = 0x1234;
  cpu.bc = 0x8765;
  cpu.de = 0x5FA1;
  cpu.hl = 0x7DD1;
  cpu.sp = 0x5766;
  cpu.pc = 0xABCD;
  
  assert_true(*getRegister(&cpu,A) == 0x34);
  assert_true(*getRegister(&cpu,F) == 0x12);
  assert_true(*getRegister(&cpu,B) == 0x65);
  assert_true(*getRegister(&cpu,C) == 0x87);
  assert_true(*getRegister(&cpu,D) == 0xA1);
  assert_true(*getRegister(&cpu,E) == 0x5F);
  assert_true(*getRegister(&cpu,H) == 0xD1);
  assert_true(*getRegister(&cpu,L) == 0x7D);
  assert_null(getRegister(&cpu,9));

  freeCPU(&cpu);
}

void test_loadCart(void ** state) {
  unsigned int cartSize;
  unsigned char *cart = loadCart("boot.gb", &cartSize);
  assert_true(cart[0] == 0x31);  
  assert_true(cartSize == 256);

  free((char *) cart);
}

void test_getNN(void ** state) {
  unsigned int cartSize;
  unsigned char *cart = loadCart("boot.gb", &cartSize);
  unsigned int nn = getNN(cart,0); 

  assert_true(nn == 0xfe31);

  free((char *) cart);
}

void test_getByte(void ** state) {
  unsigned int cartSize;
  unsigned char *cart = loadCart("boot.gb", &cartSize);
  unsigned int n = getByte(cart,0); 

  assert_true(n == 0x31);
  free((char *) cart);
}

void test_runCycle(void ** state) {
  //Load cart
  unsigned int cartSize = 0;
  const unsigned char *cart = loadCart("boot.gb", &cartSize);

  //Make CPU
  CPU cpu = initCPU();
  cpu.sp = 0x0000;
  cpu.pc = 0x0000; 
  unsigned char code = cart[cpu.pc];
  
  Op_info info = run_cycle(&cpu, cart);
 
  assert_true(cpu.pc == 0x3);
  assert_true(info.size == 3);
  assert_true(info.cycles == 12);
  
  free((char *) cart);
  freeCPU(&cpu);
}

int main (void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test(test_initCPU),
    cmocka_unit_test(test_getRegister16),
    cmocka_unit_test(test_getRegister),
    cmocka_unit_test(test_loadCart),
    cmocka_unit_test(test_getNN),
    cmocka_unit_test(test_getByte),
    cmocka_unit_test(test_runCycle)
  };

  int count_fail_tests = cmocka_run_group_tests (tests, NULL, NULL);
  return count_fail_tests;
}
