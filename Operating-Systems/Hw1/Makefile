CC = g++
CFLAGS = -g -Wall
CXXFLAGS = $(CFLAGS)
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o
TARGET = smash
RM = rm -f

# Creating the executable
$(TARGET): $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS)

# Creating the object files
commands.o: commands.cpp commands.h
	$(CC) $(CXXFLAGS) -c commands.cpp

smash.o: smash.cpp commands.h
	$(CC) $(CXXFLAGS) -c smash.cpp

signals.o: signals.cpp signals.h
	$(CC) $(CXXFLAGS) -c signals.cpp

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*
