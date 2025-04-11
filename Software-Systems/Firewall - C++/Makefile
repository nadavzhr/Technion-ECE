CXX = g++
CXXFLAGS = -g -Wall -std=c++11
CXXLINK = $(CXX)
OBJS = string.o port.o ip.o
RM = *.o libfirewall.so firewall.exe

firewall.exe: main.o libfirewall.so libinput.so
	$(CXXLINK) -g main.o -L. -lfirewall -linput -o firewall.exe

libfirewall.so: $(OBJS)
	$(CXX) -shared $(OBJS) -o libfirewall.so

main.o: main.cpp string.cpp ip.cpp port.cpp
	$(CXX) $(CXXFLAGS) -fpic -c main.cpp

string.o: string.cpp string.h
	$(CXX) $(CXXFLAGS) -fpic -c string.cpp

port.o: port.cpp port.h string.h
	$(CXX) $(CXXFLAGS) -fpic -c port.cpp

ip.o: ip.cpp ip.h string.h
	$(CXX) $(CXXFLAGS) -fpic -c ip.cpp

clean:
	rm -rf $(RM)
