COMP_FLAGS = -g -Wall -pedantic

ui.o: ui.h ui.c
	gcc -c ui.c

ui_test.o: ui_test.c
	gcc -c ui_test.c

ui_test: ui_test.o ui.o
	gcc ui_test.o ui.o $(COMP_FLAGS) -o ui_test