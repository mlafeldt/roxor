CC = gcc
CFLAGS = -Wall -Werror -O2
prefix = $(HOME)

PROG = roxor
OBJS = roxor.o

all: $(PROG)

install: $(PROG)
	install $(PROG) $(prefix)/bin

$(PROG): $(OBJS)

clean:
	$(RM) $(PROG) $(OBJS)
