CC := gcc
CFLAGS =-I./include -I./cmocka-1.1.2/include -I/usr/includeSDL2 -Wall -std=c99 -g -lSDL -D_REENTRANT -pthread -lSDL2
SRCPATH :=./src/
TESTPATH :=./test/
OBJECTS =cpu.o common.o jumptable.o mmu.o ppu.o gpu.o
TESTOBJECTS =mmu_test.o cpu_test.o jumptable_test.o

#Look for .c files in ./src
vpath %.c src
#Look for .h files in ./include
vpath %.h include 

# This exerpt is from https://makefiletutorial.com/
# Automatic Variables:
# $@ The filename representing the target.
# $% The filename element of an archive member specification.
# $< The filename of the first prerequisite.
# $? The names of all prerequisites that are newer than the target, separated by spaces.
# $^ The filenames of all the prerequisites, separated by spaces. This list has dupli-
# cate filenames removed since for most uses, such as compiling, copying, etc.,
# duplicates are not wanted.
#
# $+ Similar to $^, this is the names of all the prerequisites separated by spaces,
# except that $+ includes duplicates. This variable was created for specific situa-
# tions such as arguments to linkers where duplicate values have meaning.
#
# $* The stem of the target filename. A stem is typically a filename without its suffix.
# (We’ll discuss how stems are computed later in the section “Pattern Rules.”) Its
# use outside of pattern rules is discouraged.
.PHONY: default
default: teardown ceebee

ceebee: main.o $(OBJECTS)
	$(CC) $< $(OBJECTS) -o $@ -g -I/usr/includeSDL2 -lSDL -D_REENTRANT -pthread -lSDL2


# Compiling .o files is handled implicitly by the built in rule

.PHONY: testing
# Test build
testing: teardown $(OBJECTS) $(TESTOBJECTS)

#Look for .c testing files in ./test
vpath %.c test
jumptable_test.o: $(TESTPATH)jumptable_test.c jumptable.c
	$(CC) --coverage $(SRCPATH)jumptable.c $(CFLAGS) mmu.o cpu.o common.o $< -L./cmocka-build/src -lcmocka -o $*

cpu_test.o: $(TESTPATH)cpu_test.c cpu.c
	$(CC) --coverage $(SRCPATH)cpu.c $(CFLAGS) mmu.o jumptable.o common.o $< -L./cmocka-build/src -lcmocka -o cpu_test 

mmu_test.o: $(TESTPATH)mmu_test.c mmu.c
	$(CC) --coverage $(SRCPATH)mmu.c $(CFLAGS) jumptable.o cpu.o common.o $< -L./cmocka-build/src -lcmocka -o mmu_test

.PHONY: debug
debug: CFLAGS += -DDEBUG
debug: default

.PHONY: teardown
teardown:
	rm -f *.o

.PHONY: clean
clean: teardown
	rm -r -f cmocka-1.1.2 cmocka-1.1.2.tar.xz 
	rm -r -f cmocka-build
	rm -f ceebee 
	rm -f testing
	rm -f jumptable_test cpu_test mmu_test
	rm -f -r *.gcov *.gcda *.gcno *.info ./out
	rm -f *.txt
