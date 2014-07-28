CXX=clang++
CXXFLAGS=-g -Wall -std=c++11

INCLUDEDIRS=-I../../include/time

BINDIR=bin
SRCDIR=src

LINKDIRS=-L/opt/boost/lib

LIBRARIES=-lboost_system
LIBRARIES+=-lboost_program_options
LIBRARIES+=-lboost_random

all: set_ops2

set_ops: ${SRCDIR}/set_ops.cpp ../../src/time/timing.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDEDIRS} -o ${BINDIR}/set_ops ${SRCDIR}/set_ops.cpp ../../src/time/timing.cpp ${LINKDIRS} ${LIBRARIES}

set_ops2: ${SRCDIR}/set_ops2.cpp ../../src/time/timing.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDEDIRS} -o ${BINDIR}/set_ops2 ${SRCDIR}/set_ops2.cpp ../../src/time/timing.cpp ${LINKDIRS} ${LIBRARIES}

clean:
	find ./bin -type f -exec rm '{}' \;
