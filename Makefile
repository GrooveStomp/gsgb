#******************************************************************************
# GrooveStomp's GB Emulator
# Copyright (c) 2019 Aaron Oman (GrooveStomp)
#
# File: Makefile
# Created: 2019-06-27
# Updated: 2019-08-18
# Author: Aaron Oman
# Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
#
# This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
# you are welcome to redistribute it under certain conditions; See LICENSE for
# details.
#******************************************************************************
CC       = /usr/bin/g++
INC     += $(shell sdl2-config --cflags)
HEADERS  = $(wildcard *.hpp)
LIBS    += $(shell sdl2-config --libs) -lSDL2main
CFLAGS  += -std=c++11 -pedantic -Wall -D_GNU_SOURCE

SRC_DEP  =
SRC      = src/main.cpp src/cpu.cpp src/bus.cpp src/operand.cpp src/gb.cpp src/ram.cpp src/cartridge.cpp src/display.cpp
OBJFILES = $(patsubst %.cpp,%.o,$(SRC))
LINTFILES= $(patsubst %.cpp,__%.cpp,$(SRC)) $(patsubst %.cpp,_%.cpp,$(SRC))

RELDIR = release
RELOBJ = $(addprefix $(RELDIR)/,$(OBJFILES))
RELEXE = $(RELDIR)/gb
RELFLG = -O3

DBGDIR = debug
DBGOBJ = $(addprefix $(DBGDIR)/,$(OBJFILES))
DBGEXE = $(DBGDIR)/gb
DBGFLG = -g -Og

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
