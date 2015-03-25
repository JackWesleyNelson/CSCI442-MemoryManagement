include ../../include.mk

SOURCES = $(wildcard *.c)
PROGRAMS = $(patsubst %.c,%,$(SOURCES))

all: $(PROGRAMS)

%: %.c ../../*.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< ../../*.o

clean:
	rm -f $(PROGRAMS)
