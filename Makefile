main: main.c snake_ll.c  functions.c main_app.h
	gcc main.c functions.c snake_ll.c -pthread -o main
clean: main 
	rm -f main
run:   main
	./main
