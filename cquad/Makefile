# Compiler 
CC = gcc
CFLAGS = -Wall -O2 -std=gnu99

# Include
INCLUDE += $(GSL)

# Files
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))

# Compile
all: $(EXE)

%.o: %.c
	 $(CC) $(CFLAGS) -fopenmp $(INCLUDE) -c $<

# Link
$(EXE): $(OBJS)
	 $(CC) -fopenmp $^ -o $@ -lgsl -lgslcblas -lm

clean:
	 rm *.o
