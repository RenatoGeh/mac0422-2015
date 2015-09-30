CFLAGS = -pedantic -Wall -Wno-unused-result -std=c++0x
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

DEPS = mem_mgr.o utils.o

all: main

run: $(DEPS) main.o
	g++ $^ -o $@ $(LINKS)

main: main.cpp main.hpp mem_mgr.hpp utils.hpp
	g++ $< $(CFLAGS) $(LINKS)

mem_mgr.o: mem_mgr.cpp mem_mgr.hpp utils.hpp
	g++ $< $(CFLAGS)

utils.o: utils.cpp utils.hpp
	g++ $< $(CFLAGS)

.PHONY: clean
clean:
	rm *.out *.gch *.o mem_mgr 
	
