# Makefile for part2

part2: part2-lex.o part2.tab.o part2_helpers.o
	gcc -o $@ $^

	
part2_helpers.o : part2_helpers.c part2_helpers.h
	gcc -c -o $@ part2_helpers.c
	
part2.tab.o: part2.tab.c part2.tab.h
	gcc -c -o $@ part2.tab.c
	
part2-lex.o: part2-lex.c part2.tab.h
	gcc -c -o $@ part2-lex.c

part2-lex.c: part2.lex part2_helpers.h part2_helpers.c
	flex part2.lex

part2.tab.c part2.tab.h: part2.y
	bison -d part2.y
.PHONY: clean
clean:
	rm -f part2 part2-lex.c *.o part2.tab.c part2.tab.h  
