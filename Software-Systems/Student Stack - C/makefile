CC=gcc
CCLINK=$(CC)
CFLAGS=-g -Wall -std=c99
OBJS=student.o stack.o main.o
EXEC=prog.exe
RM=*.o *.exe

# Compile the program - ran by default
$(EXEC): $(OBJS)
	$(CCLINK) -g $(OBJS) -o prog.exe

# Stack rule
stack.o: stack.c stack.h
# Student rule
student.o: student.c student.h stack.h	
# Main program rule
main.o: main.c stack.h student.h

# Clean
# self-operated by running "make clean" inside the shell
clean:
	rm -rf $(RM)
