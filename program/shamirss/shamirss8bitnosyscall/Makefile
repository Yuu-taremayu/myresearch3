EXENAME = shamirss8bitnosyscall
SRCFILE = shamirss8bitnosyscall.c
OBJS = field_op.o ss_utils.o
CC = gcc
CFLAGS = -Wall -Wextra -g3 -O0 -MMD

all: $(EXENAME)

$(EXENAME): $(EXENAME).o $(OBJS)
	$(CC) $(CFLAGS) $(SRCFILE) $(OBJS) -o $@

field_op.o:

ss_utils.o:

clean:
	$(RM) *.share $(OBJS)
