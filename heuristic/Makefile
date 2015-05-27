# Source, Executable, Includes, Library Defines

TARGET=   dse.exe

INCL   = \
	 dse.h
  

SRCS    =  \
	dse.c  \
	fu.c \
	genetic.c 

OBJS    = $(SRCS:.c=.o)


# Compiler, Linker Defines
CC = /usr/bin/gcc
LINKER = $(CC) -L/usr/local/lib -lgsl -lgslcblas -lm
CFLAGS= -Wall -O0 -I/usr/include/gsl

debug : CFLAGS += -g -DDEBUG
RM      = /bin/rm -f


# Link all Object Files with external Libraries into Binaries
$(TARGET): $(OBJS)
	      $(LINKER) -o "$@"  $(OBJS)

# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug: $(OBJS)
	 $(LINKER) -o $(TARGET)  $(OBJS)

dse.o: dse.c
	$(CC) $(CFLAGS)  -c $< -o $@

fu.o: fu.c
	$(CC) $(CFLAGS) -c $< -o $@

genetic.o: genetic.c
	$(CC) $(CFLAGS)  -c $< -o $@


# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	 $(RM)  $(OBJS) $(TARGET) 


