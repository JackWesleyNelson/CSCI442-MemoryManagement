include ./include.mk

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

TEST_DIRS += tests/intermediate1
TEST_DIRS += tests/intermediate2
TEST_DIRS += tests/final

.PHONY: all tests_compile clean test1 test2 test grade1 grade2 grade

all: $(OBJECTS) tests_compile

tests_compile:
	for dir in $(TEST_DIRS); do $(MAKE) -C $$dir || exit 1; done

.c.o :
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(PROGRAM)
	for dir in $(TEST_DIRS); do $(MAKE) -C $$dir clean || exit 1; done

test1: all
	tests/test.sh tests/intermediate1
test2: all
	tests/test.sh tests/intermediate1 tests/intermediate2
test: all
	tests/test.sh tests/intermediate1 tests/intermediate2 tests/final
