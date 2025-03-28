# Diretories
MDIR := $(shell pwd)
WRKDIR = $(MDIR)/build
EXAMPLEDIR = $(MDIR)/example

.base:
	if ! [ -e $(WRKDIR) ]; then mkdir $(WRKDIR) ; fi;
	echo "Do not remove this file or directory..." > build/.base

vpath %.c src
vpath %.o build
vpath .base build
vpath example.c $(EXAMPLEDIR)

# C compiler:
CC = gcc

# Tool for creating static libraries:
AR = ar rv

# Error flags
ERRFLAG = -Wall -Wconversion -Wextra

# Optimization flag
OPTFLAG = -O3

# OpenMP flag
OMPFLAG = -fopenmp

# Other compilation flags
CCFLAG = -g -fPIC
LDFLAG = -g -fPIC

# Pass current working directory to the code
PKFFTDIR ?= $(MDIR)
CCFLAG += -D__PKFFTDIR__='"$(PKFFTDIR)"'

# Libraries
LIBS = -L$(PKFFTDIR) -lpkfft -lfftw3 -lm -lgsl

# Where to find include files
INCLUDES = -I$(MDIR)/include
HEADERFILES = $(wildcard ./include/*.h)

# Create object files
%.o:  %.c .base $(HEADERFILES)
	cd $(WRKDIR);$(CC) $(ERRFLAG) $(OPTFLAG) $(OMPFLAG) $(CCFLAG) $(INCLUDES) -c ../$< -o $*.o

SOURCE = error.o fft.o pkutils.o pk.o

# Source files
C_SOURCE = $(addprefix src/, $(addsuffix .c,$(basename $(SOURCE))))

# All files
C_ALL = $(SOURCE)
H_ALL = $(addprefix include/, common.h $(addsuffix .h, $(basename $(notdir $(C_ALL)))))

# Create static library
all: pkfft example

pkfft: libpkfft.a

libpkfft.a: $(SOURCE)
	$(AR) $@ $(addprefix build/, $(SOURCE))

example: example.c libpkfft.a
	$(CC) $(ERRFLAG) $(EXAMPLEDIR)/example.c $(OPTFLAG) $(OMPFLAG) $(LDFLAG) $(INCLUDES) -o $(EXAMPLEDIR)/example.o $(LIBS)

# Clean
clean:
	rm -rf $(WRKDIR);
	rm -f libpkfft.a
	rm -f $(EXAMPLEDIR)/example.o
