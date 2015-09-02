CFLAGS = -ansi -pedantic -Wall
DEBUG = false

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: ep1sh simproc

ep1sh: ep1sh.c
	gcc $< $(CFLAGS)

simproc: simproc.c
	gcc $< $(CFLAGS)

.PHONY: clean
clean:
	$(CLEAN)
	
