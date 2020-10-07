#
#   Makefile for librbt.a
#

CC=gcc
CC_OPT=-Wall -Wextra -pedantic-errors -O3 -c
OBJS=$(patsubst %.c,%.o,$(wildcard rbt_*.c))

.c.o:
	$(CC) $(CC_OPT) -o $@ $<

all: librbt.a

clean:
	rm -f *.o librbt.a

$(OBJS): rbt.h rbt_internal.h

librbt.a: $(OBJS)
	rm -f $@
	ar r $@ $+
