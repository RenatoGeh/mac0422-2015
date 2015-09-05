CFLAGS = -ansi -pedantic -Wall -Wno-unused-result
DEBUG = false
READLINEF = -lreadline -lcurses

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: ep1sh simproc

ep1sh: ep1sh.c
	gcc $< $(CFLAGS) $(READLINEF)

simproc: simproc.c
	gcc $< $(CFLAGS)

.PHONY: clean
clean:
	$(CLEAN)
	
