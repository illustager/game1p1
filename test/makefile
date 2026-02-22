CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

LIBFOLDER = ../GameGraphSolver
LIBFILE = $(LIBFOLDER)/libGameGraphSolver.a
LIBTARGET = libGameGraphSolver.a
LDFLAGS = -L$(LIBFOLDER)
INCLUDES = -I$(LIBFOLDER) -I..

.PHONY: run all clean FORCE

run: test
	./test

all: test

clean:
	$(MAKE) -C $(LIBFOLDER) clean
	rm -f test test.o

test: test.o $(LIBFILE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFILE): FORCE
	$(MAKE) -C $(LIBFOLDER) $(LIBTARGET)
