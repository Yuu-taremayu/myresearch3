EXENAME = shamirss8bit
SRCFILE = shamirss8bit.c
OBJS = field_op.o ss_utils.o
CC = gcc
CFLAGS = -Wall -Wextra -MMD

all: $(EXENAME)

$(EXENAME): $(EXENAME).o $(OBJS)
	$(CC) $(CFLAGS) $(SRCFILE) $(OBJS) -o $@

field_op.o:

ss_utils.o:

clean:
	$(RM) *.share $(OBJS)
