CC = g++
CXXFLAGS = -g -Wall -std=c++11 -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS = Bank.o atm.o account.o
TARGET = Bank
RM = rm -f

# Creating the executable
$(TARGET): $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS) -pthread

# Creating the object files
atm.o: atm.cpp atm.hpp
	$(CC) $(CXXFLAGS) -c atm.cpp -o atm.o

Bank.o: bank.cpp atm.hpp bank.hpp
	$(CC) $(CXXFLAGS) -c bank.cpp -o Bank.o

account.o: account.cpp account.hpp
	$(CC) $(CXXFLAGS) -c account.cpp -o account.o

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) $(OBJS) *~ "#" core.*
