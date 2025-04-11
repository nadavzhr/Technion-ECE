# Makefile for part3

rx-cc: part3-lex.o part3.tab.o part3_helpers.o
	g++ -o $@ $^

	
part3_helpers.o : part3_helpers.cpp part3_helpers.hpp
	g++ -c -o $@ part3_helpers.cpp
	
part3.tab.o: part3.tab.cpp part3.tab.hpp
	g++ -c -o $@ part3.tab.cpp
	
part3-lex.o: part3-lex.cpp part3.tab.hpp
	g++ -c -o $@ part3-lex.cpp

part3-lex.cpp: part3.lex part3_helpers.hpp part3_helpers.cpp
	flex part3.lex

part3.tab.cpp part3.tab.hpp: part3.ypp
	bison -d part3.ypp

.PHONY: clean
clean:
	rm -f rx-cc part3-lex.cpp *.o part3.tab.cpp part3.tab.hpp  
