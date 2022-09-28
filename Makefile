CC=gcc
DEBUG=yes
EXEC=Compile.exe
BSCP=Compile
BSCPC=bscp.c
SRC = $(wildcard *.c)
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -ansi -pedantic -g -std=c99
	LDFLAGS= -Wall -lm
else
	CFLAGS=-W -Wall -ansi -pedantic -std=c99 -O3
	LDFLAGS= -Wall -lm
endif

all: $(EXEC)

obj = $(SRC:.c=.o)
dep = $(obj:.o=.d)

$(EXEC): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(CC) $(BSCPC) -o $(BSCP)

-include $(dep)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(obj) $(EXEC) $(dep)