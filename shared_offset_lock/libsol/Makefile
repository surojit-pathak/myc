CC=gcc -pthread -g  
RM=rm
EXE=test_sol

all: sol_test.o sol.o
	$(CC) sol.c sol_test.c -o $(EXE)

clean:
	$(RM) -rf *o $(EXE)
