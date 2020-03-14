C=gcc
CFLAGS=-Wall -std=c99
CCFLAGS=-c
SRCPATH=./src/
TESTPATH=./test/
OBJECTS=main.o cpu.o common.o jumptable.o
INC=-I./include -I./lib

output: clean $(OBJECTS)
	$(C) $(CFLAGS) $(OBJECTS) -o ceebee 

main.o: $(SRCPATH)main.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)cpu.c

jumptable.o: $(SRCPATH)jumptable.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)jumptable.c

common.o: $(SRCPATH)common.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)common.c

# Test build
testing:
	$(C) $(CFLAGS) $(TESTPATH)cmocka_test.c -o testing -l cmocka

debug: CFLAGS += -DDEBUG
debug: CCFLAGS += -DDEBUG
debug: output

clean:
	rm -f $(OBJECTS)
	rm -f ceebee 
	rm -f testing
