#******************************************************************************
# File: Makefile
# Created: 2019-06-27
# Updated: 2020-12-23
# Package: gsgb
# Creator: Aaron Oman (GrooveStomp)
# Homepage: https://git.sr.ht/~groovestomp/gsgb/
# Copyright 2019 - 2020, Aaron Oman and the gsgb contributors
# SPDX-License-Identifier: AGPL-3.0-only
#******************************************************************************
CC      = /usr/bin/g++
INC     = $(shell sdl2-config --cflags)
HEADERS = $(wildcard src/*.hpp)
LIBS    = $(shell sdl2-config --libs) -lSDL2main
CFLAGS  = -std=c++17 -fno-exceptions -pedantic -Wall

SRC_DEP   =
SRC       = src/main.cpp src/cpu.cpp src/bus.cpp src/operand.cpp src/gb.cpp src/ram.cpp src/cartridge.cpp src/display.cpp
OBJFILES  = $(patsubst %.cpp,%.o,$(SRC))
LINTFILES = $(patsubst %.cpp,__%.cpp,$(SRC)) $(patsubst %.cpp,_%.cpp,$(SRC))

RELDIR = release
RELOBJ = $(addprefix $(RELDIR)/,$(OBJFILES))
RELEXE = $(RELDIR)/gb
RELFLG = -O3

DBGDIR = debug
DBGOBJ = $(addprefix $(DBGDIR)/,$(OBJFILES))
DBGEXE = $(DBGDIR)/gb
DBGFLG = -gdwarf-4 -g3 -fno-eliminate-unused-debug-symbols -fvar-tracking -fvar-tracking-assignments

TSTDIR = test
TSTSRC = $(wildcard $(TSTDIR)/*.cpp)
TSTEXE = $(patsubst $(TSTDIR)/%.cpp,$(TSTDIR)/%,$(TSTSRC))
TSTLIB = $(LIBS) -ldl
TSTOBJ = $(filter-out $(TSTDIR)/main.o,$(addprefix $(TSTDIR)/,$(OBJFILES)))

DEFAULT_GOAL := $(release)
.PHONY: clean debug docs release test

release: $(RELEXE)

$(RELEXE): $(RELOBJ)
	$(CC) -o $@ $^ $(LIBS)

$(RELDIR)/%.o: %.cpp $(HEADERS) $(SRC_DEP)
	@mkdir -p $(@D)
	$(CC) -c $*.cpp $(INC) $(CFLAGS) $(RELFLG) -o $@

debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJ)
	$(CC) -o $@ $^ $(LIBS)

$(DBGDIR)/%.o: %.cpp $(HEADERS) $(SRC_DEP)
	@mkdir -p $(@D)
	$(CC) -c $*.cpp $(INC) $(CFLAGS) $(DBGFLG) -o $@

test: $(TSTEXE)

$(TSTDIR)/%_test: $(TSTOBJ) $(TSTDIR)/%_test.o
	$(CC) -o $@ $(TSTDIR)/$*_test.o $(filter-out $(TSTDIR)/$*.o,$(TSTOBJ)) $(TSTLIB)

$(TSTDIR)/%_test.o: $(HEADERS) $(TSTDIR)/gstest.h $(TSTDIR)/%_test.cpp
	$(CC) -c $(TSTDIR)/$*_test.cpp $(INC) $(CFLAGS) $(DBGFLG) -o $@

$(TSTDIR)/%.o: %.cpp $(HEADERS)
	$(CC) -c $*.cpp $(INC) $(CFLAGS) $(DBGFLG) -o $@

runtests: test
	$(foreach exe,$(TSTEXE),./$(exe);)

clean:
	rm -rf core debug release ${LINTFILES} ${DBGOBJ} ${RELOBJ} ${TSTOBJ} ${TSTEXE} cachegrind.out.* callgrind.out.*

docs:
	doxygen .doxygen.conf
