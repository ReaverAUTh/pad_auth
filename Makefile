CC=gcc
MPICC=mpicc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

main:
	$(CC) $(CFLAGS) -o main main.c 

main_cilk:
	$(CILKCC) $(CFLAGS) -o main_cilk main_cilk.c -fcilkplus

main_openmp: 
	$(CC) $(CFLAGS) -o main_openmp main_openmp.c -fopenmp

make_graph: 
	$(CC) $(CFLAGS) -o make_graph make_graph.c -fopenmp 

main3:
	$(CC) $(CFLAGS) -o main3 main3.c 

main3_cilk:
	$(CILKCC) $(CFLAGS) -o main3_cilk main3_cilk.c -fcilkplus

main3_openmp: 
	$(CC) $(CFLAGS) -o main3_openmp main3_openmp.c -fopenmp 

main3_thread:
	$(CC) $(CFLAGS) -o main3_thread main3_thread.c -lpthread


.PHONY: clean

all: main main_cilk main_openmp make_graph main3 main3_cilk main3_openmp main3_thread 

matrix = "default"
testV3:
	@printf "\n** Testing main **\n\n"
	./main $(matrix)
	@printf "\n** Testing main_openmp **\n\n"
	./main_openmp $(matrix)
	@printf "\n** Testing main_cilk **\n\n"
	./main_cilk $(matrix)

testV4:
	@printf "\n** Testing main3 **\n\n"
	./main3
	@printf "\n** Testing main3_cilk **\n\n"
	./main3_cilk
	@printf "\n** Testing main3_openmp **\n\n"
	./main3_openmp	
	@printf "\n** Testing main3_thread **\n\n"
	./main3_thread


	
clean: 
	rm -f main main_cilk main_openmp main3 main3_cilk main3_openmp main3_thread make_graph