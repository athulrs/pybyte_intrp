byte_intrp:	main.c file_ops.o op_func.o code_init.h
		cc -g -w main.c file_ops.o op_func.o -o main
file_ops.o:	file_ops.c
	gcc -g -w -c file_ops.c
op_func.o:	op_func.c code_init.h
	gcc -g -w -c op_func.c
clean:
	rm -f main
	rm -f *.o
	rm -f *.gch
