C=gcc
CFLAGS=-Wall -std=c99
SRCPATH=./src/
OBJECTS=main.o cpu.o

output: $(OBJECTS)
	$(C) $(CFLAGS) $(OBJECTS) -o gba 

main.o: $(SRCPATH)main.c
	$(C) -c $(SRCPATH)main.c

cpu.o: $(SRCPATH)cpu.c
	$(C) -c $(SRCPATH)cpu.c

clean:
	rm $(OBJECTS)
