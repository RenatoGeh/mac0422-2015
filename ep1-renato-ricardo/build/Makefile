CFLAGS = -pedantic -Wall -Wno-unused-result -std=c99
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: ep1sh simproc

ep1sh: ep1sh.c
	gcc -o $@ $^ $(CFLAGS) $(LINKS)

simproc: simproc.c simproc.h utils.o
	gcc -o $@ $^ $(CFLAGS) $(LINKS)

utils.o: utils.c utils.h
	gcc -c $^ $(CFLAGS)

.PHONY: clean
clean:
	rm *.out *.gch *.o 
	
