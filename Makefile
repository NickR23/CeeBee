C=gcc
CFLAGS=-Wall -std=c99
CCFLAGS=-c
SRCPATH=./src/
OBJECTS=main.o cpu.o operations.o common.o
INC=-I./include -I./lib

output: clean $(OBJECTS)
	$(C) $(CFLAGS) $(OBJECTS) -o gba 

main.o: $(SRCPATH)main.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)cpu.c

operations.o: $(SRCPATH)operations.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)operations.c

common.o: $(SRCPATH)common.c
	$(C) $(INC) $(CCFLAGS) $(SRCPATH)common.c

debug: CFLAGS += -DDEBUG
debug: CCFLAGS += -DDEBUG
debug: output

clean:
	rm -f $(OBJECTS)
	rm -f gba 
