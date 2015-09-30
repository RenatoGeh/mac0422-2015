CFLAGS = -pedantic -Wall -Wno-unused-result -std=c++0x
DEBUG = false
LINKS = -lreadline -lcurses -lpthread

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

DEPS = mem_mgr.o utils.o

all: run

run: main.o $(DEPS)
	g++ $^ -o $@ $(LINKS)

main.o: main.cpp main.hpp utils.hpp mem_mgr.hpp
	g++ -c $(CFLAGS) $^

mem_mgr.o: mem_mgr.cpp mem_mgr.hpp utils.hpp
	g++ -c $(CFLAGS) $^

utils.o: utils.cpp utils.hpp
	g++ -c $(CFLAGS) $^

.PHONY: clean
clean:
	rm *.out *.gch *.o run 
	
