CFLAGS = -ansi -pedantic -Wall -Wno-unused-result
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: ep1sh simproc

utils.o: utils.c utils.h
	gcc $< $(CFLAGS) $(LINKS)

ep1sh: ep1sh.c
	gcc $< $(CFLAGS) $(LINKS)

simproc: simproc.c simproc.h utils.o
	gcc $< $(CFLAGS) $(LINKS)

.PHONY: clean
clean:
	rm *.out
	
