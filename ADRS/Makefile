# Source, Executable, Includes, Library Defines

TARGET=   adrs.exe

INCL   = \
	 adrs.h

SRCS    =  \
	adrs.c

OBJS    = $(SRCS:.c=.o)


# Compiler, Linker Defines
CC = /usr/bin/gcc
LINKER = $(CC)
CFLAGS= -ansi -pedantic -Wall -O3

debug : CFLAGS += -g -DDEBUG
RM      = /bin/rm -f


# Link all Object Files with external Libraries into Binaries
$(TARGET): $(OBJS)
	      $(LINKER) -o "$@"  $(OBJS)


# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug: $(OBJS)
	 $(LINKER) -o $(TARGET)  $(OBJS)

adrs.o: adrs.c
	$(CC) $(CFLAGS)  -c $< -o $@

# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	 $(RM)  $(OBJS) $(TARGET) 


