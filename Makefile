C=gcc
CFLAGS=-Wall -std=c99
SRCPATH=./src/
OBJECTS=main.o cpu.o operations.o common.o

output: $(OBJECTS)
	$(C) $(CFLAGS) $(OBJECTS) -o gba 

main.o: $(SRCPATH)main.c
	$(C) -c $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) -c $(SRCPATH)cpu.c

operations.o: $(SRCPATH)operations.c
	$(C) -c $(SRCPATH)operations.c

common.o: lib/common.c
	$(C) -c lib/common.c

clean:
	rm $(OBJECTS)
