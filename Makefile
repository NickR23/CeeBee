C=gcc
CFLAGS=-Wall -std=c99
CCFLAGS=-c -Wall -std=c99
SRCPATH=./src/
TESTPATH=./test/
OBJECTS=cpu.o common.o jumptable.o
TESTOBJECTS=jumptable_test.o cpu_test.o
INC=-I./include -I./cmocka-1.1.2/include

default: output teardown

output: main.o $(OBJECTS)
	$(C) $(CFLAGS) main.o $(OBJECTS) -o ceebee

main.o: $(SRCPATH)main.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)cpu.c

jumptable.o: $(SRCPATH)jumptable.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)jumptable.c

common.o: $(SRCPATH)common.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)common.c

# Test build
testing: teardown $(OBJECTS) $(TESTOBJECTS)

jumptable_test.o: $(TESTPATH)jumptable_test.c $(SRCPATH)jumptable.c
	$(C) $(INC) --coverage $(SRCPATH)jumptable.c $(CFLAGS) cpu.o common.o $(TESTPATH)jumptable_test.c -L./cmocka-build/src -lcmocka -o jumptable_test 

cpu_test.o: $(TESTPATH)cpu_test.c $(SRCPATH)cpu.c
	$(C) $(INC) --coverage $(SRCPATH)cpu.c $(CFLAGS) jumptable.o common.o $(TESTPATH)cpu_test.c -L./cmocka-build/src -lcmocka -o cpu_test 

debug: CFLAGS += -DDEBUG
debug: CCFLAGS += -DDEBUG
debug: output

teardown:
	rm -f *.o
clean: teardown
	rm -r -f cmocka-1.1.2 cmocka-1.1.2.tar.xz 
	rm -r -f cmocka-build
	rm -f ceebee 
	rm -f testing
	rm -f jumptable_test cpu_test
	rm -f -r *.gcov *.gcda *.gcno *.info ./out
