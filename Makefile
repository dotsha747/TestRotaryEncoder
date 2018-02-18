# This is the name of the project. We get it from the basename of 
# the project folder. It may contain mixed-case, and "-" as word
# separators. CamelCase is discouraged.
#
# The MAJORVER and MINORVER here are place holders; during a 
# gitpkgtool build they are substituted in-place
# by patchVersion.sh.

CWD=$(shell pwd)
PROJECT=$(shell basename $(CWD))
MAJORVER=1
MINORVER=0

# The debian package name. It may contain "-" as a word seperator
# but must be all lower case.

PACKAGE=$(shell echo $(PROJECT) | tr A-Z a-z)

# DESTDIR is overridden by debhelper.
PREFIX=$(DESTDIR)/usr
CC=g++
AR=ar
CFLAGS=-std=c++11 -O3 -I. -g
LIBS= pthread
LDOPTS=$(patsubst %, -l%, $(LIBS))
ARCH=$(shell arch)



MAINSRCS=$(wildcard src/*.cpp)
MAINOBJS=$(patsubst %.cpp, %.o, $(MAINSRCS))
MAINLIBS=pthread wiringPi  
MAINLDOPTS=
MAINEXE=testRotaryEncoder

world: all

.cpp.o:
	@echo "\t[CC] $<"
	$(CC) $(CFLAGS) -c -o $@ $<


$(MAINEXE): $(MAINOBJS)
	$(CC) -o $@ $(MAINOBJS) $(MAINLDOPTS) $(patsubst %, -l%, $(MAINLIBS))

all: $(MAINEXE) 


install: $(MAINEXE)
	install -D $(MAINEXE) $(PREFIX)/bin/$(MAINEXE)

clean: 
	rm -f $(MAINOBJS) $(MAINEXE)
	
distclean: clean


