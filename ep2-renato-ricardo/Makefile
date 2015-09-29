CFLAGS = -pedantic -Wall -Wno-unused-result -std=c++11
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

all: main

main: main.cpp main.hpp mem_mgr.o utils.o
	g++ -o $^ $(CFLAGS) $(LINKS)

mem_mgr: mem_mgr.cpp mem_mgr.hpp utils.o
	g++ -c $^ $(CFLAGS) $(LINKS)

utils.o: utils.c utils.hpp
	g++ -c $^ $(CFLAGS) $(LINKS)


.PHONY: clean
clean:
	rm *.out *.gch *.o mem_mgr 
	
