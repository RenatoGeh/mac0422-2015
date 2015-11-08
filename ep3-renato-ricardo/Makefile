CFLAGS = -pedantic -Wall -Wno-unused-result -std=c++11
DEBUG = false
LINKS = -lreadline -lcurses

ifeq ($(DEBUG), true)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

DEPS = command.o directory.o file.o regular.o utils.o

all: run

run: main.o $(DEPS)
	g++ $^ -o $@ $(LINKS)

main.o: main.cpp utils.cpp
	g++ -c $(CFLAGS) $^

command.o: command.cpp command.hpp
	g++ -c $(CFLAGS) $^

directory.o: directory.cpp directory.hpp file.hpp utils.hpp
	g++ -c $(CFLAGS) $^

file.o: file.cpp file.hpp
	g++ -c $(CFLAGS) $^

regular.o: regular.cpp regular.hpp file.hpp
	g++ -c $(CFLAGS) $^

utils.o: utils.cpp utils.hpp command.hpp
	g++ -c $(CFLAGS) $^

.PHONY: clean
clean:
	rm *.out *.gch *.o run

