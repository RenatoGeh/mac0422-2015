CFLAGS = -pedantic -Wall -Wno-unused-result
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: mem_mgr

mem_mgr: mem_mgr.cpp mem_mgr.hpp
	g++ -o $@ $^ $(CFLAGS) $(LINKS)

.PHONY: clean
clean:
	rm *.out *.gch *.o mem_mgr 
	
