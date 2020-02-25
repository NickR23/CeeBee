C=gcc
CFLAGS=-Wall -std=c99
CCFLAGS=-c
SRCPATH=./src/
OBJECTS=main.o cpu.o operations.o common.o

output: clean $(OBJECTS)
	$(C) $(CFLAGS) $(OBJECTS) -o gba 

main.o: $(SRCPATH)main.c
	$(C) $(CCFLAGS) $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) $(CCFLAGS) $(SRCPATH)cpu.c

operations.o: $(SRCPATH)operations.c
	$(C) $(CCFLAGS) $(SRCPATH)operations.c

common.o: lib/common.c
	$(C) $(CCFLAGS) lib/common.c

debug: CFLAGS += -DDEBUG
debug: CCFLAGS += -DDEBUG
debug: output

clean:
	rm -f $(OBJECTS)
	rm -f gba 
